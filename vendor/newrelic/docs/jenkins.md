# Jenkins, or: How I Learned to Stop Worrying and Love 30 Minute Build Cycles

We use [Jenkins](https://jenkins.io/) to provide the CI and build pipeline for
the PHP agent. Specifically, we use a
[Jenkins as a Service](https://source.datanerd.us/tools/jenkins-documentation)
environment provided by our Tools team, which does its work on a set
of virtual servers running in AWS in the Oregon region. You
can find it at https://phpagent-build.pdx.vm.datanerd.us/.

On a high level, Jenkins builds and tests our agents across the variety of
operating systems and architectures that we ship the PHP agent on. Each
OS/architecture is built on a different EC2 instance on AWS. These build
nodes all use [NRCAMP and NRLAMP](development_guide.md#faq) to provide their
fleet of PHPs, with the exception of Alpine Linux, which uses the
[PHP build scripts](https://source.datanerd.us/php-agent/php-build-scripts) as
a more modern alternative.

You may find some references to Hudson within the PHP agent, and more generally
around New Relic. This is because
[Jenkins is a fork of the earlier Hudson project](https://en.wikipedia.org/wiki/Hudson_(software)#Hudson%E2%80%93Jenkins_split),
and the PHP agent was initially developed before that fork. You can treat them
as interchangeable for the purposes of agent development.

## Build nodes

The full list of configured build nodes can be found here:
https://phpagent-build.pdx.vm.datanerd.us/configure.

In practice, for day to day development the important nodes are the ones
with nrcamp in their name.

There are two sets of glibc Linux nodes: CentOS and Ubuntu. The CentOS nodes
are the ones that we use to actually build the agent that we ship to customers,
since CentOS 5 is an appropriately lowest common denominator; the Ubuntu nodes
are used primarily for packaging (since we need `dpkg` to build Debian
packages) and secondarily to build the agent on a slightly newer compiler. (In
practice, it's likely that you will have a newer compiler still on your local
development environment, so this is somewhat less important than it once was.)

### Adding a build node

You've decided to expand the nodes you perform tests on, great! It's rather
straight forward to add an additional AMI in Jenkins. Go to
[configure system](https://phpagent-build.pdx.vm.datanerd.us/configure) and
scroll down to the section labeled Cloud. There are two Amazon EC2 sections in
here each with their own name. One is labeled "Build Nodes" and represents
nodes used to execute build jobs, while the other is labeled "us-west-2" and
represents generic and one-off nodes. Add your AMI to the most appropriate 
category.

To add an additional AMI select the Add button at the bottom of your section of
interest. Fill in the following fields

- Description: Name the node something recognizable so others will know its
purpose
- AMI ID: An Amazon Machine Image Identifier
- Instance type: Choose from the drop-down your desired AWS EC2 instance. Note
that not every instance type supports every AMI
- Security group names: Choose from the list of security groups found in the
AWS UI under EC2 Dashboard Security Groups section
- Remote FS root: Home directory for Jenkins. Recommended setting is
`/home/hudson`
- Remote user: User for Jenkins to login as. Recommended setting is `hudson`
- Remote SSH Port: 22
- Labels: Set to a desired name. This is the name that will be used in Jenkins
jobs. [For example](https://phpagent-build.pdx.vm.datanerd.us/view/All/job/php-pull-request/)
the five configurations in this job are labels. Whitespace is allowed but is
counted as part of the name
- Usage: Change if you want to restrict what jobs use your node
- Idle termination time: Set to 0 if you want the instance to never terminated.
Otherwise set it to the desired minutes
- [Advanced] Number of Executors: The number of jobs that can run in parallel
on one instance of your node
  - Note: Jenkins will spin up as many instances as it needs to run as many
  jobs at once, unless instance cap is set
- [Advanced] Tags: Set the following tags
  - `Name` : `<Same as Description>`
  - `owning_team` : `native_agents`
  - `product` : `agents`
  - `environment` : `development`
  - `department` : `product`

### Updating an existing build node

Scenarios: The tools team has decided to rotate out GHE keys, or you need to
update nrcamp/nrlamp.

For this section you will primarily be working in AWS.

1. Login to AWS as `newrelic-ee`, Oregon region. Go to the AMI section and
filter by `owning_team : native_agents` to find the AMI
of interest
2. Launch an EC2 instance
   * Instance Type: Does not matter
   * Configure Instance Details: Do not touch
   * Add Storage: Do not touch
   * Add Tags: Add the same tags you would when [Adding a build node](#adding-a-build-node)
   * Configure Security Group: Choose a security group that best suits
   your needs to update the build node
   * Launch: Proceed without a key pair
3. Login to the instance. Refer to the
[logging into build nodes](#logging-into-build-nodes) section for more
information
4. Make the desired changes to the running instance
5. Check to confirm the changes made work when rebooted
6. Stop the running instance
7. Right click on the stopped instance and select `Create Image`
   * Refer to the original AMI's `AMI Name` to get an idea of how to name this
   image. Example `PHP Ubuntu 10.04 32`
   * Note: AWS does not allow two AMIs to have the exact same `AMI Name`. If
   you really want the same exact name you will need to first deregister the
   original AMI. Only do this if you are confident you no longer need it
8. Go to [configure system](https://phpagent-build.pdx.vm.datanerd.us/configure)
and update the `AMI ID` of the build node that was changed

### Logging into build nodes

All build nodes are accessible via SSH. For nodes that are already running you
can pull the IP address out of each node
[by clicking on its name](https://phpagent-build.pdx.vm.datanerd.us/computer)
in this list and then clicking on its `Configure` link; the Private DNS is the
IP to connect to.

If the node is not currently running you have two options to access the node of
interest.

- Preferred method: Go to [Manage Nodes](https://phpagent-build.pdx.vm.datanerd.us/computer)
and select the node of interest from the dropbox button 
"Provision via Build Nodes". This will create a node in the current list which 
you can access by finding its IP using the above method for a running node.
This method is preferred because it will automatically tag your EC2 instance
properly. Be mindful that your node will terminate itself when its idle timeout
limit has been reached. At the time of writing this document it has been set to
480 minutes. That is configurable from
[Configure System](https://phpagent-build.pdx.vm.datanerd.us/configure).

- Generate an EC2 instance of the node of interest by going to AWS and
launching it directly. Filter on the tag `owning_team : native_agents` to see
the PHP AMIs. Note you will have to manually tag your EC2 instance and remember
to terminate it when no longer needed.

There are two ways to login to a node.

- Have an account in the agent team Puppet configuration. In
short, you'll need to have an entry with your SSH public key in
[common.yaml](https://source.datanerd.us/newrelic/agent-puppet/blob/master/hieradata/common.yaml).
You can follow the example of
[Alan's PR to add himself](https://source.datanerd.us/newrelic/agent-puppet/pull/638).

- Login as `hudson` user. Use the hudson key named Hudson JaaS Key,
`MIIEow..Ba0g9`.

Once you log in, it's worth noting that build jobs run as the `hudson` user,
and you can find the workspace for each job in `~hudson/workspace`. You
can also build the agent in your own home directory if you're trying to track
down a failure that only manifests in Jenkins, with two caveats:

1. Our `/home` partitions on most build nodes are quite limited in size. Please
   try to clean up after you're done building the agent; it's bigger than you
   think.
2. The default shell on each build node does not set up the `PATH` to give you
   an NRCAMP/NRLAMP enabled build environment. You'll probably want to do this
   before building anything:

    ```sh
    export PATH=/opt/nr/camp/bin:/opt/nr/lamp/bin:/usr/local/go/bin:$PATH
    ```
3. When logging in as the `hudson` user the environment variables `HUDSON_USER`
   and `HUDSON_PASS` need to be set in order to clone from GHE. These values 
   are found in
   [global properties](https://phpagent-build.pdx.vm.datanerd.us/configure)
   
Further detail is also available
[on Confluence](https://newrelic.atlassian.net/wiki/spaces/eng/pages/41287713/Cross-Platform+Sanity+Tests+xPlatform#Cross-PlatformSanityTests(xPlatform)-HowtoAccesstotheNodes),
if you find something missing below. (Please add it to this document too!)

### Alpine

Our Alpine build nodes don't use NRCAMP or NRLAMP, as we never ported NRCAMP
and NRLAMP to support non-glibc flavours of Linux, and by the time we added
Alpine support we were trying to kill NRCAMP and NRLAMP with fire.

As noted in the introduction, these use the
[php-build-scripts](https://source.datanerd.us/php-agent/php-build-scripts) to
provide the various PHP versions we support.

We don't, at present, have a standard method or process for updating the Alpine
build node.

### macOS

The macOS build node is a fairly normal NRCAMP/NRLAMP node like the others,
with a key exception: the node is on a physical Mac Mini instead
of being virtualized in AWS. This means that building NRCAMP or NRLAMP
using their Jenkins jobs will make the macOS build node inoperative until the
build completes successfully.

## Important jobs

This isn't intended to be a complete list, but it does summarise the key jobs
you'll need as a developer.

### [`php-pull-request`](https://phpagent-build.pdx.vm.datanerd.us/view/All/job/php-pull-request)

This job runs on every commit in every open PR, or when you tell it to by
entering `ok jenkins` as a comment on an open PR. It builds the agent on every
build node and runs a set of tests — on some OSes, these aren't much more than
basic sanity tests, while on Linux, these basically include everything up to
running most of our unit tests under Valgrind.

The script that drives this is
[`hudson/build-pull-request.sh`](https://source.datanerd.us/php-agent/php_agent/blob/master/hudson/build-pull-request.sh).

### `*-bigchange-*`

These jobs are configured identically to the release jobs, except that you can
change which fork and branch they build on by changing the configuration of
each job. This is exceptionally useful when you want release packages for an
in-development branch, either for testing or to provide a special build to a
customer.

There is no lock preventing multiple people from using these at once, so to
avoid conflicts, you should **clearly** state in the #php-agent Slack channel
when you intend to take control of these jobs, and again state when you are
done with them.

## Common issues

### Unable to use GHE on CentOS 5 nodes

Attempting to use GIT when logged into one of the CentOS 5 nodes may result in
failure due to a missing environment variable.

CentOS 5 is a special case. In the
[configuration](https://phpagent-build.pdx.vm.datanerd.us/configure) setting
there is an environment variable `GIT_SSH` that is set for GIT to work
properly. That can be found in the `init script` section configuring the AMI.

### macOS builds are failing

This usually means that an NRCAMP or NRLAMP build failed. Check them, fix them,
and try again.

### NRCAMP and/or NRLAMP changes need to be deployed to build nodes

Because all EC2 instances are spawned from AMIs any permanent changes need to
have a new AMI created.

1. Create EC2 instances from primary build node's, CentOS5 and FreeBSD, AMIs
   * Refer to [updating an existing build node](#updating-an-existing-build-node)
   to create an instance
2. Login to the instance
   * Refer to the [logging into build nodes](#logging-into-build-nodes)
   section for more information
3. Checkout [nrcamp](https://source.datanerd.us/php-agent/nrcamp)
4. Checkout [nrlamp](https://source.datanerd.us/php-agent/nrlamp)
5. Execute the runbuildit script in each repo `bash -x ./runbuildit.sh`
6. If successful shutdown each node and update the AMIs

### Jenkins job sits and spins, waiting for a node to come online

When Jenkins starts a job it will attempt to reuse / trigger new nodes to run
for each label in the job's configuration matrix. Looking in the console log
you can see exactly what Jenkins is attempting to do. You may see several
`doesn’t have label` and `is offline` messages as it scans its list of 
[nodes](https://phpagent-build.pdx.vm.datanerd.us/computer).

What Jenkins is doing is looking at each of the existing nodes to see if any
match the label it was told to run on. Jenkins will skip those with unmatched
labels or ones that match but are not online just yet. Jenkins will eventually
give up on an offline node if it is unable to SSH in to start the Jenkins agent.

What is happening behind the scenes with an offline node is Jenkins had spun up
an EC2 instance of the desired AMI. This node takes some time to come up in
order to SSH in and start the Jenkins agent. Occasionally the node takes too
long to start up and connect. By then Jenkins has determined this offline node
to be a failure.

So far the only known reason why Jenkins might sit and spin is because the
`instance cap` of the AMI is configured to 1, allowing a maximum of one node to
be created. Removing the value and leaving it empty in the
[configuration](https://phpagent-build.pdx.vm.datanerd.us/configure) setting
will allow Jenkins to spawn a second EC2 and rescan the list. This time Jenkins
will find a working node and continue to run.

You also have the option of manually kick starting an EC2 instance from Jenkins.
Go to the list of [nodes](https://phpagent-build.pdx.vm.datanerd.us/computer)
and select the node of desire from the `Provision via Build Nodes` dropdown.
This will spawn a new node and cause Jenkins to continue to scan the list since
a change has occurred. Jenkins should pick up this node and continue running.

## Misc

### Where can I find backups?

* The AMIs on AWS have been copied to the `N. California` region in case any
AMI accidentally gets deregistered in the Oregon region. Copying to different
regions in AWS is trivial and only require a right click and "Copy AMI" to be
replicated in a new region. Be sure to re-tag the copied AMI.

* For machines that could not be automatically migrated from vSphere to AWS
[this](https://s3.console.aws.amazon.com/s3/buckets/php-agent-vsphere-volumes/?region=us-east-1&tab=overview)
S3 bucket contains the vSphere volumes used in the migration. This data is
backed up in case an issue arises in any of the already created AWS AMIs.

### How do I reseed jobs using groovy files?

The PHP agent's Jenkins' instance was reseeded once in 2018 for testing,
followed by 2016. We don't reseed often, nor are all of our jobs represented
with groovy files.

If you want to reseed jobs using the repository's groovy files use the
[refresh-jenkins-jobs](https://phpagent-build.pdx.vm.datanerd.us/view/All/job/refresh-jenkins-jobs)
job.  This job, when built with parameters, will look for all .groovy files
contained within hudson/jobs/ and recreate those jobs in Jenkins.

### Jenkins builds fail. I think it isn't my fault. I don't have the time or motivation to debug Jenkins. What should I do?

Sometimes Jenkins fails in strange ways. Reasons for this can be, for example, 
corrupt workspaces or the lack of disk space on build nodes.

There are two brute-force practices that often fix those issues: restarting 
build nodes and wiping out workspaces. Before you do this, *make sure that no 
PR builder jobs are running*. To do this, navigate to the [pull-request-parallel-top](https://phpagent-build.pdx.vm.datanerd.us/job/php-pull-request-parallel-top/)
Jenkins job and verify that no build is currently in progress.

* _Restart build nodes_. This can be done for builds that run on AWS nodes
  (Alpine, CentOS and FreeBSD). You cannot do this for MacOS builds. Restarting
  an AWS node effectively wipes out the workspace and will also fix swap/memory
  issues on the node. 

  1. Navigate to the [node setup](https://phpagent-build.pdx.vm.datanerd.us/computer/) 
     page in Jenkins.
  2. Search for a node of the platform that causes problems. If there is no
     node left, you're finished.
  3. Click on one node of the platform that causes problems.
  4. Click _Disconnect_ in the list on the left. Confirm by clicking _Yes_.
  5. Continue with (i).

  New AWS nodes will be provisioned automatically when a new PR build is 
  triggered. The first PR build might take a longer time, as it has to spin up
  the new AWS nodes.

* _Wipe out the workspace_. This should be done for MacOS builds, which do not
  run on AWS nodes.

  1. Navigate to the Jenkins build that failed (e. g. [here](https://phpagent-build.pdx.vm.datanerd.us/job/php-pull-request-parallel-5-6/label=macos106-64-nrcamp/)). 
  2. Click _Workspace_ in the list on the left and make it expand.
  3. In the expanded list, click _Wipe Out Current Workspace_.
