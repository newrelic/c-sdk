package newrelic

import (
	"testing"
	"time"

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

func TestHarvestTriggerCustomBuilder(t *testing.T) {

	reply := &ConnectReply{}

	methods := &collector.DataMethods{
		ErrorEventData:    &collector.ReportPeriod{InSeconds: 1},
		AnalyticEventData: &collector.ReportPeriod{InSeconds: 1},
		CustomEventData:   &collector.ReportPeriod{InSeconds: 1},
		SpanEventData:     &collector.ReportPeriod{InSeconds: 1},
	}

	reply.DataMethods = methods

	triggerChannel := make(chan HarvestType)
	cancelChannel := make(chan bool)
	customTrigger := customTriggerBuilder(reply, collector.MinimumReportPeriod, time.Millisecond)

	go customTrigger(triggerChannel, cancelChannel)

	// A customTrigger is a collection of five goroutines that have
	// invoked a ticker according to a prescribed report period. Go
	// makes no fairness or deadline guarantees about how it schedules
	// its goroutines, so under overloaded conditions, the goroutine
	// handling the HarvestErrorEvent might be scheduled twice before
	// any other goroutine in the collection.

	// That said, we would like to test that each kind of harvest
	// event occurs when a customTrigger is built.  We also don't want
	// this test to fail under overloaded conditions.

	// Read HarvestType events for at least five reporting periods, or
	// until we've seen at least one of each type.
	m := make(map[HarvestType]int)
	for {
		event := <-triggerChannel
		m[event] = m[event] + 1
		if len(m) >= 5 {
			break
		}
	}

	if m[HarvestErrorEvents] < 1 {
		t.Fatal("HarvestErrorEvents event did not occur")
	}

	if m[HarvestTxnEvents] < 1 {
		t.Fatal("HarvestTxnEvents event did not occur")
	}

	if m[HarvestCustomEvents] < 1 {
		t.Fatal("HarvestCustomEvents event did not occur")
	}

	if m[HarvestSpanEvents] < 1 {
		t.Fatal("HarvestSpanEvents event did not occur")
	}

	if m[HarvestDefaultData] < 1 {
		t.Fatal("HarvestDefaultData event did not occur")
	}

	// Drain any pending events before canceling, otherwise the cancel
	// message won't be processed and the test will deadlock.
Outer:
	for {
		select {
		case <-triggerChannel:
			// Do nothing.
		default:
			break Outer
		}
	}

	cancelChannel <- true

	// Ensure that the cancellation is processed, and that nothing is
	// sent to the trigger channel after cancellation has been confirmed.
	<-cancelChannel

	timer := time.NewTimer(time.Duration(2*collector.MinimumReportPeriod) * time.Millisecond)
	select {
	case <-timer.C:
		// Excellent; we didn't receive anything on triggerChannel.
		break
	case event := <-triggerChannel:
		t.Fatalf("Unexpected event %v received after triggers were cancelled", event)
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
