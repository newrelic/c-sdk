<?php declare(strict_types = 1);

namespace Newrelic\NewRelicApi;

use GuzzleHttp\Client;
use Psr\Http\Message\ResponseInterface;
use TreeHouse\NewRelicApi\Error\ApiErrorHandler;
use TreeHouse\NewRelicApi\Error\ApiException;
use TreeHouse\NewRelicApi\ResponseParser\JsonResponseParser;
use TreeHouse\NewRelicApi\ResponseParser\ResponseParserInterface;
use TreeHouse\NewRelicApi\ResponseParser\XmlResponseParser;
use TreeHouse\NewRelicApi\Client\Filter;

class InsightsApiClient
{
    /**
     * Default URL if none is set
     */
    const API_URL = 'https://staging-insights-api.newrelic.com/v1';
    const ENDPOINT_QUERY = '/accounts/%s/query';


    /**
     * @var string
     */
    private $apiUrl=false;

    /**
     * @var string
     */
    private $apiKey;

    /**
     * @var string
     */
    private $accountId;

    /**
     * @var Client
     */
    private $client;

    /**
     * @var ApiErrorHandler
     */
    private $errorHandler;

    /**
     * @var Filter[]
     */
    private $filters;

    /**
     * @var string
     */
    private $format = 'json';


    /**
     * @var ResponseParserInterface
     */
    public $responseParser;

    /**
     * @param string $apiKey
     */
    public function __construct(string $apiKey, string $accountId)
    {
        $this->apiKey = $apiKey;
        $this->accountId = $accountId;
        $this->client = new Client();
        $this->errorHandler = new ApiErrorHandler();
        $this->setResponseParser();
    }

    /**
     * @param Filter $filter
     */
    public function addFilter(Filter $filter)
    {
        $this->filters[$filter->getName()] = $filter->getValue();
    }

    private function createFilter($name, $value)
    {
        return new Filter($name, $value);
    }


    public function query($nrql)
    {
        $this->addFilter(
            $this->createFilter('nrql', $nrql)
        );
        return $this->request();
    }
    /**
     * @param string $uri     The URI of the API endpoint.
     *                        Do not include the .json or .xml suffix, use setFormat to change the format instead.
     * @param string $method  The HTTP method to use (GET, POST, PUT, DELETE)
     * @param array  $headers Any additional headers to send with the request.
     *
     * @throws ApiException
     *
     * @return ResponseInterface
     */
    public function request(string $method = 'GET', array $headers = []) : ResponseInterface
    {
        $uri = sprintf(static::ENDPOINT_QUERY, $this->accountId);

        $headers = array_merge($headers, ['X-Query-Key' => $this->apiKey]);
        $filters = !empty($this->filters) ? http_build_query($this->filters) : null;
        // $url = sprintf('%s%s.%s?%s', self::API_URL, $uri, $this->format, $filters);
        $url = sprintf('%s%s?%s', $this->getApiUrl(), $uri, $filters);
        $response = $this->client->request($method, $url, ['headers' => $headers]);
        $this->errorHandler->handle($response);

        return $response;
    }

    public function getApiUrl()
    {
        if($this->apiUrl)
        {
            return $this->apiUrl;
        }
        return static::API_URL;
    }

    public function setApiUrl($value)
    {
        $this->apiUrl = $value;
    }


    /**
     * Sets the ResponseParser to use based on the chosen format.
     */
    private function setResponseParser()
    {
        switch (strtolower($this->format)) {
            case 'json':
                $this->responseParser = new JsonResponseParser();
                break;
            default:
                throw new \RuntimeException(sprintf(
                    'Unknown format "%s" set in ApiClient. Use either "json" or "xml".',
                    $this->format
                ));
        }
    }
}
