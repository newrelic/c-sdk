package newrelic

import (
	"testing"

	"newrelic/collector"
)

func TestHarvestTriggerGet(t *testing.T) {

	reply := &ConnectReply{}

	trigger := getHarvestTrigger("1234", reply)

	if trigger == nil {
		t.Fatal("No harvest trigger")
	}
}

func TestHarvestTriggerGetCustom(t *testing.T) {

	reply := &ConnectReply{}

	reportPeriod := &collector.ReportPeriod{
		InSeconds: 15,
	}

	methods := &collector.DataMethods{
		ErrorEventData:    reportPeriod,
		AnalyticEventData: reportPeriod,
		CustomEventData:   reportPeriod,
	}

	reply.DataMethods = methods

	trigger := getHarvestTrigger("1234", reply)

	if trigger == nil {
		t.Fatal("No custom harvest trigger created")
	}

}

func TestFasterHarvestWhitelistReplyBuilder(t *testing.T) {
	reply1 := fasterHarvestWhitelistReplyBuilder(7)

	if 7 != reply1.DataMethods.ErrorEventData.InSeconds {
		t.Fatal("ErrorEventData value is incorrect")
	}

	if 7 != reply1.DataMethods.AnalyticEventData.InSeconds {
		t.Fatal("AnalyticEventData value is incorrect")
	}

	if 7 != reply1.DataMethods.CustomEventData.InSeconds {
		t.Fatal("CustomEventData value is incorrect")
	}

	if 7 != reply1.DataMethods.SpanEventData.InSeconds {
		t.Fatal("SpanEventData value is incorrect")
	}
}
