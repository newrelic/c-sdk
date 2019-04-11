#include "tlib_php.h"

#include "php_agent.h"
#include "php_call.h"
#include "php_hash.h"
#include "fw_drupal_common.h"

tlib_parallel_info_t parallel_info
    = {.suggested_nthreads = -1, .state_size = 0};

static void test_single_extract_module_name_from_hook_and_hook_function(
    const char* hook_function_name,
    char* hook_name,
    char* expected_module_name) {
  char* module = 0;
  size_t module_len = 0;

  module_invoke_all_parse_module_and_hook_from_strings(
      &module, &module_len, hook_name, strlen(hook_name), hook_function_name,
      strlen(hook_function_name));

  tlib_pass_if_str_equal("Extracted Correct Module Name", module,
                         expected_module_name);
  nr_free(module);
}

static void test_module_name(void) {
  int i = 0;
  int number_of_fixtures = 0;

  // a set of three string sets.
  // fixtures[i][0] = The full PHP function name of the hook
  // fixtures[i][1] = The portion of the function name that's the hook name
  // fixtures[i][2] = The module name we expect to be extracted

  char* fixtures[][3]
      = {{"modulename_hookname", "hookname", "modulename"},
         {"foo_bar", "bar", "foo"},
         {"help_help", "help", "help"},
         {"locale_locale", "locale", "locale"},
         {"menu_menu", "menu", "menu"},
         {"ckeditor_skin_ckeditor_skin", "ckeditor_skin", "ckeditor_skin"},
         {"context_context", "context", "context"},
         {"views_form_views_form", "views_form", "views_form"},
         {"atlas_statistics_atlas_statistics", "atlas_statistics",
          "atlas_statistics"},
         {"atlas_statistics_atlas_stat", "atlas_stat", "atlas_statistics"}};

  number_of_fixtures = sizeof(fixtures) / sizeof(fixtures[0]);

  for (i = 0; i < number_of_fixtures; i++) {
    test_single_extract_module_name_from_hook_and_hook_function(
        fixtures[i][0], fixtures[i][1], fixtures[i][2]);
  }
}

void test_main(void* p NRUNUSED) {
  test_module_name();
}
