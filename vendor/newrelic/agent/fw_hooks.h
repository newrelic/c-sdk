/*
 * This file contains framework hooking code for all frameworks.
 */
#ifndef FW_HOOKS_HDR
#define FW_HOOKS_HDR

/*
 * Ensure nr_framework_classification_t is defined.
 */
#include "php_execute.h"

extern void nr_cakephp_enable_1(TSRMLS_D);
extern void nr_cakephp_enable_2(TSRMLS_D);
extern nr_framework_classification_t nr_cakephp_special_1(
    const zend_op_array* op_array TSRMLS_DC);
extern nr_framework_classification_t nr_cakephp_special_2(
    const zend_op_array* op_array TSRMLS_DC);

extern void nr_codeigniter_enable(TSRMLS_D);

extern int nr_drupal_is_framework(nrframework_t fw);
extern void nr_drupal_enable(TSRMLS_D);

extern void nr_drupal8_enable(TSRMLS_D);
extern void nr_joomla_enable(TSRMLS_D);
extern void nr_kohana_enable(TSRMLS_D);
extern void nr_laravel_enable(TSRMLS_D);
extern void nr_magento1_enable(TSRMLS_D);
extern void nr_magento2_enable(TSRMLS_D);
extern void nr_mediawiki_enable(TSRMLS_D);
extern void nr_symfony1_enable(TSRMLS_D);
extern void nr_symfony2_enable(TSRMLS_D);
extern void nr_silex_enable(TSRMLS_D);
extern void nr_slim_enable(TSRMLS_D);
extern void nr_wordpress_enable(TSRMLS_D);
extern void nr_yii_enable(TSRMLS_D);
extern void nr_zend_enable(TSRMLS_D);
extern void nr_fw_zend2_enable(TSRMLS_D);

extern void nr_fw_aura_enable(TSRMLS_D);
extern void nr_fw_fuel_enable(TSRMLS_D);
extern void nr_fw_lithium_enable(TSRMLS_D);
extern void nr_fw_micromvc_enable(TSRMLS_D);
extern void nr_fw_phpbb_enable(TSRMLS_D);
extern void nr_fw_phpixie_enable(TSRMLS_D);
extern void nr_fw_phreeze_enable(TSRMLS_D);
extern void nr_fw_sellvana_enable(TSRMLS_D);
extern void nr_fw_senthot_enable(TSRMLS_D);
extern void nr_fw_typo3_enable(TSRMLS_D);
extern void nr_fw_moodle_enable(TSRMLS_D);
extern void nr_fw_atutor_enable(TSRMLS_D);
extern void nr_fw_dokeos_enable(TSRMLS_D);
extern void nr_fw_expression_engine_enable(TSRMLS_D);
extern void nr_fw_dokuwiki_enable(TSRMLS_D);
extern void nr_fw_phpnuke_enable(TSRMLS_D);
extern void nr_fw_silverstripe_enable(TSRMLS_D);
extern void nr_fw_sugarcrm_enable(TSRMLS_D);
extern void nr_fw_xoops_enable(TSRMLS_D);
extern void nr_fw_e107_enable(TSRMLS_D);

/* Libraries. */
extern void nr_doctrine2_enable(TSRMLS_D);
extern void nr_guzzle3_enable(TSRMLS_D);
extern void nr_guzzle4_enable(TSRMLS_D);
extern void nr_guzzle6_enable(TSRMLS_D);
extern void nr_mongodb_enable(TSRMLS_D);
extern void nr_phpunit_enable(TSRMLS_D);
extern void nr_predis_enable(TSRMLS_D);
extern void nr_zend_http_enable(TSRMLS_D);

#endif /* FW_HOOKS_HDR */
