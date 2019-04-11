# These should be cleared when we release
NEWRELIC_COLLECTOR_HOST := staging-collector.newrelic.com
NEWRELIC_LICENSE_KEY    := 07a2ad66c637a29c3982469a3fe8d1982d002c4a
WHITE_LIST              := {"5e454a7d7f634b0466e58602c66d5dd01c578e92":10,"94a0ba329358e811a5aed99cac701a0ec77bbf67":10}
# license "94a0ba329358e811a5aed99cac701a0ec77bbf67" Production Account 1691007
# license "5e454a7d7f634b0466e58602c66d5dd01c578e92" Production Account 98015
NEWRELIC_COLLECTOR_KEYS := {"nonLaspProd":"951809161a1a07acc7da550284de0b6b09320be2","nonLaspStag":"9851c5a3b9bd2e4295c4a8712120548889d99296","laspProd":"5940d824e8244b19798fef5b6a70b8b73804484a","laspStag":"1cccc807e3eb81266a3f30d9a58cfbbe9d613049","mostSecureStag":"20a5bbc045930ae7e15b530c8a9c6b7c5a918c4f","securityToken":"ffff-ffff-ffff-ffff"}

# Production account: 820618
# Test communication against a non-LASP enabled production account
# license: 951809161a1a07acc7da550284de0b6b09320be2
#
# Staging Account: 17833
# Test communication against a non-LASP enabled staging account
# license: 9851c5a3b9bd2e4295c4a8712120548889d99296
#
# Production Account: 1939623
# Test communication against a LASP enabled production account */
# Policies: All policies set to "most-secure" (enabled:false)
# Valid Token: "ffff-ffff-ffff-ffff"
# license: 5940d824e8244b19798fef5b6a70b8b73804484a
#
# Staging Account: 10002849
# Test communication against LASP enabled staging accounts */
# Policies: All policies set to "most-secure" (enabled:false)
# Valid Token: "ffff-ffff-ffff-ffff"
# license: 1cccc807e3eb81266a3f30d9a58cfbbe9d613049
#
# Staging Account: 10005915
# Policies: All policies set to "most-secure" (enabled:false), job_arguments has required:true
# Valid Token: "ffff-ffff-ffff-ffff"
# license: 20a5bbc045930ae7e15b530c8a9c6b7c5a918c4f
