import newrelic.jenkins.extensions

def ownerAndRepo = 'php-agent/php_agent'
def branchSpec = '*/master'

// How many builds to keep and for how long.
def daysToKeep = 30
def numToKeep = -1  // no limit

use(extensions) {
  freeStyleJob('php-static-analysis') {
    description 'Run Clang Analyzer on the PHP agent.'

    repository(ownerAndRepo, branchSpec) {
      clean(true)
      pruneBranches(true)
      recursiveSubmodules(true)
    }

    label('alpine-3-3-1')

    triggers {
      cron('H 1 * * *')
    }

    steps {
      environmentVariables {
	env('PATH', '/usr/local/go/bin:$PATH')
      }

      shell("./hudson/static-analysis.sh")
    }

    publishers {
      warnings(['GNU C Compiler 4 (gcc)'])
      archiveArtifacts('reports/scanbuild/**/*')

      groovyPostBuild(readFileFromWorkspace('hudson/report-to-insights.groovy'))

      extendedEmail('php-agent@newrelic.com', '$DEFAULT_SUBJECT', '${SCRIPT, template="php-agent-html.template"}') {
	trigger('Failure')
	configure { node ->
	  node / contentType << 'text/html'
	  node / matrixTriggerMode << 'ONLY_PARENT'
	}
      }
    }
  }
}
