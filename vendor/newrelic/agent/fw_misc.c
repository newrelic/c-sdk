#include "php_agent.h"
#include "php_user_instrument.h"
#include "php_execute.h"
#include "fw_support.h"
#include "fw_hooks.h"

/*
 * Misc php frameworks, or suggested frameworks, gleaned from these sites:
 *   https://newrelic.atlassian.net/browse/PHP-493
 *   http://www.hhvm.com/blog/2813/we-are-the-98-5-and-the-16
 *   http://en.wikipedia.org/wiki/PHP_frameworks#PHP
 *   http://www.phpframeworks.com
 *   http://www.techempower.com/benchmarks/#section=data-r8&hw=i7&test=json&l=sg
 *
 * See here for some notes on how you might inflate these frameworks as
 * part of a more thorough instrumentation of the various frameworks.
 *
 *   https://newrelic.atlassian.net/wiki/display/eng/PHP+Agent+Framework+Detection+Blitz
 *
 * Some notes about some putative frameworks:
 *   Yaf     is written as a PHP extension
 *   phalcon is written as a PHP extension
 *
 * Frameworks[sic] suggested by the HHVM testing effort:
 *   assetic an asset management framework, not a web framework
 *   doctrine2 is an ORM layer.
 *   idiorm is an ORM layer
 *   phpmyadmin is an admin layer for mysql; not really a framework
 *   twig is a template compilation layer, compiles templates down to php
 */

/*
 * See:
 *   http://auraphp.com
 *   http://auraphp.com/framework
 */
void nr_fw_aura_enable(TSRMLS_D) {
  NR_UNUSED_TSRMLS;
}

/*
 * See:
 *   http://fuelphp.com
 */
void nr_fw_fuel_enable(TSRMLS_D) {
  NR_UNUSED_TSRMLS;
}

/*
 * See:
 *   https://github.com/UnionOfRAD
 */
void nr_fw_lithium_enable(TSRMLS_D) {
  NR_UNUSED_TSRMLS;
}

/*
 * See:
 *   https://github.com/Xeoncross/micromvc
 */
void nr_fw_micromvc_enable(TSRMLS_D) {
  NR_UNUSED_TSRMLS;
}

/*
 * See:
 *   www.phpbb.com
 *   https://github.com/phpbb/phpbb
 *   https://www.phpbb.com/downloads/
 */
void nr_fw_phpbb_enable(TSRMLS_D) {
  NR_UNUSED_TSRMLS;
}

/*
 * See:
 *   http://phpixie.com
 */
void nr_fw_phpixie_enable(TSRMLS_D) {
  NR_UNUSED_TSRMLS;
}

/*
 * See:
 *   http://phreeze.com
 *   http://phreeze.com/phreeze/documentation/installation.php
 */
void nr_fw_phreeze_enable(TSRMLS_D) {
  NR_UNUSED_TSRMLS;
}

/*
 * See
 *   http://www.sellvana.com
 * This is being developed by some people that worked on magento.
 * As of 02/21/2014 Robert signed up through sellvana.com
 * for a development copy, got a link to
 *   https://bitbucket.org/sellvana/core
 * and then only looked at index.php
 * (The code is either OSL'ed or GPL'ed.)
 */
void nr_fw_sellvana_enable(TSRMLS_D) {
  NR_UNUSED_TSRMLS;
}

/*
 * See (Indonesian)
 *   http://www.senthot.com
 */
void nr_fw_senthot_enable(TSRMLS_D) {
  NR_UNUSED_TSRMLS;
}

/*
 * See:
 *   http://docs.typo3.org/flow/TYPO3FlowDocumentation/Quickstart/Index.html
 */
void nr_fw_typo3_enable(TSRMLS_D) {
  NR_UNUSED_TSRMLS;
}

/*
 * Moodle CMS (course management software)
 * Modular Object-Oriented Dynamic Learning Environment
 * See:
 *   http://download.moodle.org/
 * Needed to install the php zip library.  rrh tried:
 *   pecl-5.5-no-zts install zip
 *   add extension=zip.so to /opt/nr/etc/php-common/php.ini
 * but the moodle installer still wasn't happy with that.
 *
 */
void nr_fw_moodle_enable(TSRMLS_D) {
  NR_UNUSED_TSRMLS;
}

/*
 * ATutor CMS (course management software)
 * Open Source LMS (learning management software)
 * See:
 *   http://atutor.ca
 *
 * Installation got to step3 of the installation and then appeared to deadlock
 * The file names it loads are very generically named.
 */
void nr_fw_atutor_enable(TSRMLS_D) {
  NR_UNUSED_TSRMLS;
}

/*
 * Dokeos (Belgium online learning site)
 *   www.dokeos.com
 * There's a community edition, need to register to get email which has a link
 * to download the zip file.
 *  http://www.dokeos.com/dokeos-community-edition-download/
 *  http://www.dokeos.com/dokeosce30.zip
 */
void nr_fw_dokeos_enable(TSRMLS_D) {
  NR_UNUSED_TSRMLS;
}

/*
 * Expession Engine
 *   www.ellislab.com
 *
 * This appears to be built on top of CodeIgniter, since
 *    system/codeigniter/system/core/CodeIgniter.php
 * is loaded before any expressionengine specific code is loaded
 * Consequently, uses of this framework get marked as "CodeIgniter"
 *
 * There's a limited community edition, need to register to get access.
 * Downloaded:
 *   https://store.ellislab.com/?ACT=243
 * See:
 *   http://ellislab.com/expressionengine/user-guide/installation/installation.html
 */
void nr_fw_expression_engine_enable(TSRMLS_D) {
  NR_UNUSED_TSRMLS;
}

/*
 * See:
 *   docuwiki.org
 *   http://download.dokuwiki.org/
 */
void nr_fw_dokuwiki_enable(TSRMLS_D) {
  NR_UNUSED_TSRMLS;
}

/*
 * See:
 *   www.phpnuke.org
 *   https://www.phpnuke.org/modules.php?name=Release
 */
void nr_fw_phpnuke_enable(TSRMLS_D) {
  NR_UNUSED_TSRMLS;
}

/*
 * See:
 *   www.silverstripe.org
 *   http://www.silverstripe.org/stable-download
 */
void nr_fw_silverstripe_enable(TSRMLS_D) {
  NR_UNUSED_TSRMLS;
}

/*
 * See:
 *   www.sugarcrm.com
 *   http://www.sugarcrm.com/download
 *   http://www.sugarcrm.com/thank-you-downloading-sugarcrm-community-edition
 *   http://sourceforge.net/projects/sugarcrm/files/latest/download?source=files
 */
void nr_fw_sugarcrm_enable(TSRMLS_D) {
  NR_UNUSED_TSRMLS;
}

/*
 * See:
 *   http://sourceforge.net/projects/xoops/
 */
void nr_fw_xoops_enable(TSRMLS_D) {
  NR_UNUSED_TSRMLS;
}

/*
 * See:
 *   http://e107.org/download/
 */
void nr_fw_e107_enable(TSRMLS_D) {
  NR_UNUSED_TSRMLS;
}
