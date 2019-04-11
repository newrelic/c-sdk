<?php
require 'vendor/autoload.php';
use Newrelic\NewRelicApi\ApiClient;
use Newrelic\NewRelicApi\InsightsApiClient;
use TreeHouse\NewRelicApi\Error\ApiException;

function createFilter($name, $value)
{
    return new \TreeHouse\NewRelicApi\Client\Filter($name, $value);
}

function getInsightsClient($apiKey, $applicationId)
{
    $client = new InsightsApiClient($apiKey, $applicationId);
    $client->setApiUrl(getInsightsApiUrl());
    return $client;
}

function exitWithUsage()
{
    echo "ERROR: Bailing\n";
    echo "    USAGE: php test.php [START_DATE_TIME] [END_DATE_TIME]","\n";
    echo "    USAGE: ./doIt.bash [START_DATE_TIME] [END_DATE_TIME]","\n";
    echo "Time in YYYY-MM-DD HH:MM:SS format\n\n";
    exit(1);
}

/**
 * Makes sure we're getting a date in the expected format
 */
function formatDatetime($string)
{
    return date('Y-m-d H:i:s', strToTime($string));
}

function getTransactionNrql($start_time, $end_time)
{
    return sprintf("
        SELECT count(*)
        FROM Transaction
        FACET errorType
        WHERE appName='C-Agent Test App'
            AND errorType = 'StressTransactionError'
            AND name LIKE '%%StressTransaction_1'
        SINCE '%s'
        UNTIL '%s'
        ",formatDatetime($start_time), formatDatetime($end_time));
}

function getTransactionErrorNrql($start_time, $end_time)
{
    return sprintf("
        SELECT count(*)
        FROM TransactionError
        FACET `error.class`
        WHERE appName='C-Agent Test App'
            AND `error.class` = 'StressTransactionError'
        SINCE '%s'
        UNTIL '%s'
        ",formatDatetime($start_time), formatDatetime($end_time));
}

function runFacetQuery($client, $nrql)
{
    $response     = $client->query($nrql);
    $results      = $client->responseParser->parse($response);
    $facets       = array_merge([], $results->facets);
    return $facets;
}

function reportCounts($facets)
{
    echo "\n";
    echo "EventType", "", "          ErrorType","              ","      Count","\n";
    echo "-------------------------------------------------------\n";
    $counts = [];
    foreach($facets as $type=>$facet)
    {
        $counts[] = $facet->results[0]->count;
        echo $type, str_repeat(' ', 19 - strlen($type)), $facet->name,"\t", $facet->results[0]->count,"\t", "\n";
    }
    echo "-------------------------------------------------------\n";
    if(count(array_unique($counts)) !== 1)
    {
        echo "WARNING: Transaction event count and TransactionError event count don't match.\n";
    }

}

function reportNrql($ranNrql)
{
    echo "\nRan NRQL: \n\n";
    foreach($ranNrql as $nrql)
    {
        $nrql = preg_replace('%^\s*%m', '', $nrql);
        echo $nrql, "\n\n";
    }
}

function exitWithEnvVar($envVar)
{
    echo "ERROR: Please set a `$envVar` env var\n";
    exit;
}

function getInsightsApiUrl()
{
    $key = isset($_SERVER['NEW_RELIC_INSIGHTS_API_URL']) ? $_SERVER['NEW_RELIC_INSIGHTS_API_URL'] : NULL;
    if($key)
    {
        return $key;
    }
    return 'https://staging-insights-api.newrelic.com/v1';
}

function getInsightsAccountId()
{
    $key = isset($_SERVER['NEW_RELIC_INSIGHTS_ACCOUNT_ID']) ? $_SERVER['NEW_RELIC_INSIGHTS_ACCOUNT_ID'] : NULL;
    if(!$key)
    {
        exitWithEnvVar('NEW_RELIC_INSIGHTS_ACCOUNT_ID');
    }
    return $key;
    return '432507';
}

function getInsightsApiKey()
{
    $key = isset($_SERVER['NEW_RELIC_INSIGHTS_API_KEY']) ? $_SERVER['NEW_RELIC_INSIGHTS_API_KEY'] : NULL;
    if(!$key)
    {
        exitWithEnvVar('NEW_RELIC_INSIGHTS_API_KEY');
    }
    return $key;
}

function main($argv)
{
    $script_name = array_shift($argv);
    $start_time  = array_shift($argv);
    $end_time    = array_shift($argv);
    $facets      = [];

    if(!$start_time || !$end_time)
    {
        exitWithUsage();
    }

    $client = getInsightsClient(getInsightsApiKey(), getInsightsAccountId());
    echo "Using: ", $client->getApiUrl(), "\n";

    $nrql = getTransactionNrql($start_time, $end_time);
    $facets['Transaction']      = runFacetQuery($client, $nrql)[0];
    $ranNrql[] = $nrql;

    $nrql = getTransactionErrorNrql($start_time, $end_time);
    $facets['TransactionError'] = runFacetQuery($client, $nrql)[0];
    $ranNrql[] = $nrql;

    reportCounts($facets);
    reportNrql($ranNrql);

}
main($argv);
