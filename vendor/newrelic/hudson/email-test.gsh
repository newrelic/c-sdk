// Groovy script for testing custom emails without running an actual build.
// 
// Usage
// -----
// - Ensure the job is configured to use the email-ext plugin and
//   the message content has been set to invoke the desired template.
// - Set projectName variable to the name of the job.
// - Set p.recipientList to your email.
// - Paste this script into http://pdx-hudson.datanerd.us/script
// - Click "run".

import hudson.model.StreamBuildListener
import hudson.plugins.emailext.ExtendedEmailPublisher
import java.io.ByteArrayOutputStream

def projectName = "PHP_Flat_Makefile"
Jenkins.instance.copy(Jenkins.instance.getItem(projectName), "$projectName-Testing");

def project = Jenkins.instance.getItem(projectName)
try {
  def testing = Jenkins.instance.getItem("$projectName-Testing")

  // See the http://javadoc.jenkins-ci.org/hudson/model/Job.html#getLastBuild()
  // for other ways to get builds.
  //def build = project.lastBuild
  def build = project.lastFailedBuild

  def baos = new ByteArrayOutputStream()
  def listener = new StreamBuildListener(baos)

  testing.publishersList.each() { p ->
    println(p)
    if (p instanceof ExtendedEmailPublisher) {
      // modify the properties as necessary here
      p.recipientList = 'php-agent@newrelic.com'

      // run the publisher
      p.perform((AbstractBuild<?,?>)build, null, listener)
      // print out the build log from ExtendedEmailPublisher
      println(new String(baos.toByteArray(), "UTF-8"))
    }
  }
} finally {
  if (testing != null) {
    // cleanup the test job
    testing.delete()
  }
}
