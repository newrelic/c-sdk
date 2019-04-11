import newrelic.jenkins.extensions

// How many builds to keep and for how long.
def daysToKeep = 30
def numToKeep = -1 // no limit

use(extensions) {
  matrixJob('php-pull-request') {
    description """Build the daemon, agent, axiom tests, and related tools<br>
in accordance with our pull request guidelines, which are documented<br>
in the README.md file. Additionally, run the axiom tests.
"""

    logRotator(daysToKeep, numToKeep, -1, -1)

    // Ensure the cross agent tests are updated too.
    scm {
      git {
        remote {
          datanerd 'php-agent/php_agent'
          name 'origin'
          refspec '+refs/pull/*:refs/remotes/origin/pr/*'
        }

        branch '${sha1}'

        clean(true)
        pruneBranches(true)
        recursiveSubmodules(true)
      }
    }

    triggers {
      pullRequest {
        admins([
                 'aharvey',
                 'cpine',
                 'earnold',
                 'mlaspina',
                 'rvanderwal',
                 'will'
               ])
        orgWhitelist('php-agent')
        triggerPhrase('ok jenkins')
        permitAll()
        useGitHubHooks()
      }
    }

    // Parent build runs on master
    label('master')
    axes {
      label('label',
            'alpine-3-3-1',
            'centos5-32-nrcamp',
            'centos5-64-nrcamp',
            'macos106-64-nrcamp',
            'freebsd100-64-nrcamp')
    }

    steps {
      shell('/bin/bash hudson/build-pull-request.sh')
    }

    publishers {
      groovyPostBuild(readFileFromWorkspace('hudson/report-to-insights.groovy'))

      extendedEmail('php-agent@newrelic.com', '$DEFAULT_SUBJECT', '${SCRIPT, template="php-agent-html.template"}') {
        trigger('Failure')
        configure { node ->
          node / contentType << 'text/html'
          node / matrixTriggerMode << 'ONLY_PARENT'
        }
      }
    }

    configure { project ->
      project / buildWrappers / 'matrixtieparent.BuildWrapperMtp'(plugin: 'matrixtieparent@1.1') {
        labelName('master')
      }
    }
  }
}
