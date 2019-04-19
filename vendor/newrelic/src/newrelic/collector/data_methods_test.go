package collector

import (
	"testing"
)

func TestDataMethodsGetValue(t *testing.T) {

	reportPeriod := &ReportPeriod{
		InSeconds: 15,
	}

	methods := &DataMethods{
		ErrorEventData:    reportPeriod,
		AnalyticEventData: reportPeriod,
		CustomEventData:   reportPeriod,
	}

	if methods.GetValue("ErrorEventData") != 15 {
		t.Errorf("Incorrect report period for error events = %v", methods.GetValue("ErrorEventData"))
	}

	if methods.GetValue("AnalyticEventData") != 15 {
		t.Errorf("Incorrect report period for analytic events = %v", methods.GetValue("AnalyticEventData"))
	}

	if methods.GetValue("CustomEventData") != 15 {
		t.Errorf("Incorrect report period for custom events = %v", methods.GetValue("CustomEventData"))
	}
}

func TestDataMethodsNotAField(t *testing.T) {

	reportPeriod := &ReportPeriod{
		InSeconds: 15,
	}

	methods := &DataMethods{
		ErrorEventData:    reportPeriod,
		AnalyticEventData: reportPeriod,
		CustomEventData:   reportPeriod,
	}
	if methods.GetValue("NotAField") != -1 {
		t.Errorf("Incorrect handling of non-member struct field = %v", methods.GetValue("NotAField"))
	}
}

func TestDataMethodsGetValueOrDefault(t *testing.T) {

	reportPeriod := &ReportPeriod{
		InSeconds: -1000,
	}

	methods := &DataMethods{
		ErrorEventData:    reportPeriod,
		AnalyticEventData: reportPeriod,
		CustomEventData:   reportPeriod,
	}

	if methods.GetValueOrDefault("ErrorEventData") != DefaultReportPeriod {
		t.Errorf("Incorrect report period for error events = %v", methods.GetValue("ErrorEventData"))
	}

	if methods.GetValueOrDefault("AnalyticEventData") != DefaultReportPeriod {
		t.Errorf("Incorrect report period for analytic events = %v", methods.GetValue("AnalyticEventData"))
	}

	if methods.GetValueOrDefault("CustomEventData") != DefaultReportPeriod {
		t.Errorf("Incorrect report period for custom events = %v", methods.GetValue("CustomEventData"))
	}
}

func TestDataMethodsGetValueOrDefaultAsMin(t *testing.T) {

	reportPeriod := &ReportPeriod{
		InSeconds: 1,
	}

	methods := &DataMethods{
		ErrorEventData:    reportPeriod,
		AnalyticEventData: reportPeriod,
		CustomEventData:   reportPeriod,
	}

	if methods.GetValueOrDefault("ErrorEventData") != MinimumReportPeriod {
		t.Errorf("Incorrect report period for error events = %v", methods.GetValue("ErrorEventData"))
	}

	if methods.GetValueOrDefault("AnalyticEventData") != MinimumReportPeriod {
		t.Errorf("Incorrect report period for analytic events = %v", methods.GetValue("AnalyticEventData"))
	}

	if methods.GetValueOrDefault("CustomEventData") != MinimumReportPeriod {
		t.Errorf("Incorrect report period for custom events = %v", methods.GetValue("CustomEventData"))
	}
}

func TestDataMethodsGetValueOrDefaultAsValue(t *testing.T) {

	reportPeriod := &ReportPeriod{
		InSeconds: 120,
	}

	methods := &DataMethods{
		ErrorEventData:    reportPeriod,
		AnalyticEventData: reportPeriod,
		CustomEventData:   reportPeriod,
	}

	if methods.GetValueOrDefault("ErrorEventData") != 120 {
		t.Errorf("Incorrect report period for error events = %v", methods.GetValue("ErrorEventData"))
	}

	if methods.GetValueOrDefault("AnalyticEventData") != 120 {
		t.Errorf("Incorrect report period for analytic events = %v", methods.GetValue("AnalyticEventData"))
	}

	if methods.GetValueOrDefault("CustomEventData") != 120 {
		t.Errorf("Incorrect report period for custom events = %v", methods.GetValue("CustomEventData"))
	}
}

func TestDataMethodsGetValueOrDefaultAsMax(t *testing.T) {

	reportPeriod := &ReportPeriod{
		InSeconds: 1000,
	}

	methods := &DataMethods{
		ErrorEventData:    reportPeriod,
		AnalyticEventData: reportPeriod,
		CustomEventData:   reportPeriod,
	}

	if methods.GetValueOrDefault("ErrorEventData") != MaximumReportPeriod {
		t.Errorf("Incorrect report period for error events = %v", methods.GetValue("ErrorEventData"))
	}

	if methods.GetValueOrDefault("AnalyticEventData") != MaximumReportPeriod {
		t.Errorf("Incorrect report period for analytic events = %v", methods.GetValue("AnalyticEventData"))
	}

	if methods.GetValueOrDefault("CustomEventData") != MaximumReportPeriod {
		t.Errorf("Incorrect report period for custom events = %v", methods.GetValue("CustomEventData"))
	}
}

func TestDataMethodsNil(t *testing.T) {

	var methods *DataMethods
	methods = nil

	if methods.GetValue("ErrorEventData") != -1 {
		t.Errorf("Incorrect report period for nil DataMethods = %v", methods.GetValue("ErrorEventData"))
	}
}

func TestDataMethodsGetEmpty(t *testing.T) {

	methods := &DataMethods{}

	if methods.GetValue("ErrorEventData") != -1 {
		t.Errorf("Incorrect report period for nil field of DataMethods = %v", methods.GetValue("ErrorEventData"))
	}
}

func TestDataMethodsAllEqualTo(t *testing.T) {

	reportPeriod := &ReportPeriod{
		InSeconds: 15,
	}

	methods := &DataMethods{
		ErrorEventData:    reportPeriod,
		AnalyticEventData: reportPeriod,
		CustomEventData:   reportPeriod,
	}

	if !methods.AllEqualTo(15) {
		t.Errorf("Incorrect equality result for DataMethods")
	}
}

func TestDataMethodsAllEqualToNil(t *testing.T) {

	var methods *DataMethods
	methods = nil

	if methods.AllEqualTo(15) {
		t.Errorf("Incorrect equality result for DataMethods")
	}
}

func TestDataMethodsAllEqualToEmpty(t *testing.T) {

	methods := &DataMethods{}

	if methods.AllEqualTo(15) {
		t.Errorf("Incorrect equality result for DataMethods")
	}
}
