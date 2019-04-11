import groovy.json.JsonOutput
import java.net.HttpURLConnection

// When invoked as a post-build step, report the build results
// to Insights for use in the PHP Agent build dashboard.
//
// See: https://staging-insights.newrelic.com/accounts/432507/dashboards/2024
//
// To test this script substitute the uses of the manager global
// with the commented code to get a reference to job and execute
// this script using the Jenkins Console. (http://pdx-hudson.datanerd.us/console)

def project = manager.build.getProject().getRootProject()
def build = manager.build

//def project = Jenkins.instance.getItem('PHP_PullRequest_Build')
//def build = project.lastBuild

def payload = [
  'eventType': 'JenkinsJobResult',
  'jobName': project.getDisplayName(),
  'nodeName': node = build.getEnvVars()['label'],
  'buildNumber': build.number,
  'buildResult': build.getResult().toString()
]

def conn = 'https://staging-insights-collector.newrelic.com/v1/accounts/432507/events'.toURL().openConnection()
conn.setDoOutput(true)
conn.setRequestMethod('POST')
conn.setRequestProperty('Content-Type', 'application/json')
conn.setRequestProperty('X-Insert-Key', '4VR4m_YZjCqcycYwZHlDPy0Y0uuOnTD0')
conn.outputStream.withWriter { Writer w -> w << JsonOutput.toJson(payload) }

def response = conn.inputStream.withReader { Reader r -> r.text }
manager.listener.logger.println("Insights responsed with " + response)
//println("Insights responsed with " + response)
