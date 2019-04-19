package collector

import (
	"reflect"
)

const (
	MinimumReportPeriod int = 5
	DefaultReportPeriod int = 60
	MaximumReportPeriod int = 300
)

type ReportPeriod struct {
	InSeconds int `json:"report_period_in_seconds"`
}
type DataMethods struct {
	ErrorEventData    *ReportPeriod `json:"error_event_data"`
	AnalyticEventData *ReportPeriod `json:"analytic_event_data"`
	CustomEventData   *ReportPeriod `json:"custom_event_data"`
	SpanEventData     *ReportPeriod `json:"span_event_data"`
}

// Get values by name from a DataMethods type. Since the DataMethods are coming
// from the collector over the wire in a JSON blob, it is possible that they may
// not be well-formed.
func (methods *DataMethods) GetValue(field string) int {
	if methods != nil {
		m := reflect.ValueOf(*methods)
		field := m.FieldByName(field)

		if field.IsValid() { // It's possible the supplied string is not a struct field.
			fieldValue := field.Elem()
			if fieldValue.IsValid() { // It's possible that the struct field is nil.
				reportPeriod := fieldValue.Field(0).Int()
				return int(reportPeriod)
			}
		}
	}

	// Returns -1 if no value was found.
	return -1
}

// Adhere to the spec in that:
// The report_period_in_seconds property MUST NOT be set below 5 seconds,
// or above 300 seconds (5 minutes).
//   - If the property is not set, is not a number, or set to 0 or below,
//     then use the default of 60 seconds.
//   - If the property is in the range [1,4] then use the minimum of 5 seconds.
//   - If the property is set above 300, then use the maximum of 300 seconds.
func (methods *DataMethods) GetValueOrDefault(field string) int {
	if value := methods.GetValue(field); value < 1 {
		return DefaultReportPeriod
	} else if value <= MinimumReportPeriod {
		return MinimumReportPeriod
	} else if value <= MaximumReportPeriod {
		return value
	} else {
		return MaximumReportPeriod
	}
}

// Return true if all report periods are equal to the period supplied
func (methods *DataMethods) AllEqualTo(period int) bool {
	reportPeriod := &ReportPeriod{
		InSeconds: period,
	}

	testMethods := &DataMethods{
		ErrorEventData:    reportPeriod,
		AnalyticEventData: reportPeriod,
		CustomEventData:   reportPeriod,
	}

	return reflect.DeepEqual(methods, testMethods)
}
