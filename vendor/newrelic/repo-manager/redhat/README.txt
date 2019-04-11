How to update the newrelic-repo RPM
===================================

Perform these steps on the host **build-centos5-64-01**

1. Remove the previous newrelic-repo.*.rpm

2. Make your changes to the newrelic.repo

3. Update the Release attribute in the newrelic-repo.spec

4. Build the RPM: rpmbuild -bb newrelic-repo.spec

5. Sign the RPM: rpm --addsign newrelic-repo-5-*.noarch.rpm

6. Check the signature: rpm --checksig newrelic-repo-5-*.noarch.rpm

7. Add the new newrelic-repo.*.rpm file to Git, commit, and push
