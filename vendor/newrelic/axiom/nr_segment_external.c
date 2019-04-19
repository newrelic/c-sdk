#include "nr_axiom.h"

#include <stdio.h>

#include "nr_header.h"
#include "nr_segment_external.h"
#include "nr_segment_private.h"
#include "util_strings.h"
#include "util_url.h"

/*
 * Purpose : Set all the typed external attributes on the segment.
 */
static void nr_segment_external_set_attrs(
    nr_segment_t* segment,
    const nr_segment_external_params_t* params,
    char* external_guid) {
  nr_segment_external_t attrs = {0};

  if (params->uri) {
    attrs.uri = nr_url_clean(params->uri, nr_strlen(params->uri));
  }
  attrs.library = params->library;
  attrs.procedure = params->procedure;
  attrs.transaction_guid = external_guid;

  nr_segment_set_external(segment, &attrs);

  nr_free(attrs.uri);
}

/*
 * Purpose : Create metrics for a completed external call and set the segment
 *           name.
 *
 * Metrics created during this call
 * ----------------------------------------------------------------------------
 * External/all                                                Unscoped Always
 * External/{host}/all                                         Scoped   non-CAT
 * External/{host}/all                                         Unscoped CAT
 * ExternalApp/{host}/{external_id}/all                        Unscoped CAT
 * ExternalTransaction/{host}/{external_id}/{external_txnname} Scoped   CAT
 *
 * Metrics created based on External/all (in nr_txn_create_rollup_metrics)
 * ----------------------------------------------------------------------------
 * External/allWeb                                             Unscoped Web
 * External/allOther                                           Unscoped non-Web
 *
 * Segment name
 * ----------------------------------------------------------------------------
 * External/{host}/all                                                  non-CAT
 * ExternalTransaction/{host}/{external_id}/{external_txnname}          CAT
 *
 * These metrics are dictated by the spec located here:
 * https://source.datanerd.us/agents/agent-specs/blob/master/Cross-Application-Tracing-PORTED.md
 */
static void nr_segment_external_create_metrics(nr_segment_t* segment,
                                               const char* uri,
                                               const char* external_id,
                                               const char* external_txnname) {
  char buf[1024] = {0};
  size_t buflen = sizeof(buf);
  const char* domain;
  int domainlen = 0;

#define ADD_METRIC(M_scoped, ...)     \
  snprintf(buf, buflen, __VA_ARGS__); \
  nr_segment_add_metric(segment, buf, M_scoped);

  domain = nr_url_extract_domain(uri, nr_strlen(uri), &domainlen);
  if ((NULL == domain) || (domainlen <= 0)
      || (size_t)domainlen >= (buflen - 256)) {
    domain = "<unknown>";
    domainlen = nr_strlen(domain);
  }

  /* Rollup metric.
   *
   * This has to be created on the transaction in order to create
   * External/allWeb and External/allOther and to calculate
   * externalDuration later on.
   */
  nrm_force_add(segment->txn->unscoped_metrics, "External/all",
                nr_time_duration(segment->start_time, segment->stop_time));

  if (external_id && external_txnname) {
    /* Metrics in case of CAT */
    ADD_METRIC(false, "External/%.*s/all", domainlen, domain);
    ADD_METRIC(false, "ExternalApp/%.*s/%s/all", domainlen, domain,
               external_id);
    ADD_METRIC(true, "ExternalTransaction/%.*s/%s/%s", domainlen, domain,
               external_id, external_txnname);
  } else {
    /* Metrics in case of not-CAT */
    ADD_METRIC(true, "External/%.*s/all", domainlen, domain);
  }

  /* buf now contains the name of the scoped metric. This is also used as
   * the segment name. */
  segment->name = nr_string_add(segment->txn->trace_strings, buf);
}

static bool nr_segment_external_rollup(nr_segment_t* segment) {
  nr_segment_t* parent = NULL;
  nr_segment_t* rollup = NULL;

  if (nrunlikely(NULL == segment)) {
    return false;
  }

  parent = segment->parent;

  if (NULL == parent) {
    return false;
  }

  /* Find a sibling suitable for rollup. This sibling must:
   *
   *  1. Not be the segment itself.
   *  2. Not have children.
   *  3. Have the same name as the segment.
   *
   * If there are more siblings for which 1-3 are true, the sibling with the
   * latest stop time is used for rollup.
   */
  for (size_t i = 0; i < nr_vector_size(&parent->children); i++) {
    nr_segment_t* sibling = nr_vector_get(&parent->children, i);

    if (NULL == sibling) {
      continue;
    }

    if (sibling == segment) {
      continue;
    }

    if (sibling->name != segment->name) {
      continue;
    }

    if (nr_vector_size(&sibling->children) > 0) {
      continue;
    }

    if (NULL != rollup && rollup->stop_time > sibling->stop_time) {
      continue;
    }

    rollup = sibling;
  }

  if (NULL == rollup) {
    return false;
  }

  /* Add duration and metrics to the rollup, then destroy the segment.  */

  rollup->stop_time = segment->stop_time;

  while (nr_vector_size(segment->metrics) > 0) {
    void* sm;

    nr_vector_pop_front(segment->metrics, &sm);
    nr_vector_push_back(rollup->metrics, sm);
  }

  nr_segment_children_remove(&parent->children, segment);

  nr_segment_destroy(segment);

  rollup->txn->segment_count -= 1;

  return true;
}

bool nr_segment_external_end(nr_segment_t* segment,
                             const nr_segment_external_params_t* params) {
  char* external_id = NULL;
  char* external_txnname = NULL;
  char* external_guid = NULL;
  bool rv = false;

  if (NULL == segment || NULL == params || NULL == segment->txn) {
    return false;
  }

  if (params->encoded_response_header) {
    nr_header_outbound_response(segment->txn, params->encoded_response_header,
                                &external_id, &external_txnname,
                                &external_guid);
  }

  nr_segment_external_set_attrs(segment, params, external_guid);

  rv = nr_segment_end(segment);

  nr_segment_external_create_metrics(segment, params->uri, external_id,
                                     external_txnname);

  if (params->do_rollup) {
    nr_segment_external_rollup(segment);
  }

  nr_free(external_id);
  nr_free(external_txnname);
  nr_free(external_guid);

  return rv;
}
