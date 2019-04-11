package newrelic

import (
	"encoding/json"
	"time"

	"newrelic/collector"
	"newrelic/log"
	"newrelic/secrets"
)

type whitelist map[string]int

var whiteListStruct whitelist

// A harvest trigger function. Each App has one of these, and it sends
// HarvestType messages to the given channel to initiate a harvest for that app.
// When an AppHarvest is closed, it sends a `true` event over the trigger
// function's cancel channel.
type HarvestTriggerFunc func(trigger chan HarvestType, cancel chan bool)

// Given a reply, determine whether the configuration requires the same
// reporting period across all data reporting.
func (reply *ConnectReply) isHarvestAll() bool {
	if reply != nil {
		dataMethods := reply.DataMethods

		return dataMethods.AllEqualTo(collector.DefaultReportPeriod)
	}

	// If a well-formed ConnectReply is not supplied, assume a default reporting
	// period across all data reporting.
	return true
}

// A convenience function to create a harvest trigger function which triggers
// a harvest event of type t once every duration.
func triggerBuilder(t HarvestType, duration time.Duration) func(
        chan HarvestType, chan bool) {
	return func(trigger chan HarvestType, cancel chan bool) {
		ticker := time.NewTicker(duration)
		for {
			select {
			case <-ticker.C:
				trigger <- t
			case <-cancel:
				ticker.Stop()
				// Send a message back to the cancel channel confirming that the
				// ticker has been stopped.
				cancel <- true
				return
			}
		}
	}
}

// To create a group of goroutines that may be cancelled by sending a single
// message on a single channel, this function:
// - Creates a cancel channel for the goroutine function f.
// - Starts the goroutine.
// - Returns the newly-created cancel channel so that it may be added to a
//   broadcast group.
func startGroupMember(f func(chan HarvestType, chan bool), trigger chan
                      HarvestType) chan bool {
	cancel := make(chan bool)
	go f(trigger, cancel)
	return cancel
}

// In some cases, five different kinds of data are harvested at five different
// periods.  In such cases, build the comprehensive harvest trigger that adheres
// to such a configuration.
func customTriggerBuilder(reply *ConnectReply, reportPeriod int,
                          units time.Duration) func(chan HarvestType, chan bool) {
	methods := reply.DataMethods

	defaultTrigger := triggerBuilder(HarvestDefaultData,
		time.Duration(reportPeriod)*units)
	analyticTrigger := triggerBuilder(HarvestTxnEvents,
		time.Duration(methods.GetValueOrDefault("AnalyticEventData"))*units)
	customTrigger := triggerBuilder(HarvestCustomEvents,
		time.Duration(methods.GetValueOrDefault("CustomEventData"))*units)
	errorTrigger := triggerBuilder(HarvestErrorEvents,
		time.Duration(methods.GetValueOrDefault("ErrorEventData"))*units)
	spanTrigger := triggerBuilder(HarvestSpanEvents,
		time.Duration(methods.GetValueOrDefault("SpanEventData"))*units)

	return func(trigger chan HarvestType, cancel chan bool) {
		broadcastGroup := make([]chan bool, 0)

		broadcastGroup = append(broadcastGroup, startGroupMember(
		                            defaultTrigger, trigger))
		broadcastGroup = append(broadcastGroup, startGroupMember(
		                            analyticTrigger, trigger))
		broadcastGroup = append(broadcastGroup, startGroupMember(
		                            customTrigger, trigger))
		broadcastGroup = append(broadcastGroup, startGroupMember(
		                            errorTrigger, trigger))
		broadcastGroup = append(broadcastGroup, startGroupMember(
		                            spanTrigger, trigger))

		// This function listens for the cancel message and then broadcasts it
		// to all members of the broadcastGroup.
		go func() {
			<-cancel
			for _, c := range broadcastGroup {
				c <- true
				// As we need to send a confirmation that all trigger functions
				// have been cancelled, we'll wait for this function to confirm
				// that the cancellation has been processed.
				<-c
			}

			// Send a confirmation that the cancellation has been processed,
			// since we know from the loop above that all functions in the
			// broadcast group have been cancelled.
			cancel <- true
		}()
	}
}

// This function creates a connect reply for
// the customTriggerBuilder function.
func fasterHarvestWhitelistReplyBuilder(seconds int) *ConnectReply {
	reply := &ConnectReply{}
	methods := &collector.DataMethods{
		ErrorEventData:    &collector.ReportPeriod{InSeconds: seconds},
		AnalyticEventData: &collector.ReportPeriod{InSeconds: seconds},
		CustomEventData:   &collector.ReportPeriod{InSeconds: seconds},
		SpanEventData:     &collector.ReportPeriod{InSeconds: seconds},
	}
	reply.DataMethods = methods
	return reply
}

// This function checks the customAppEventHarvestTimesInSeconds map for a key
// match.  If none is found, return nil to indicate the customer is not on
// the whitelist.  If we do have a key match, return a custom trigger function.
// The report period for events will be the value in customAppEventHarvestTimesInSeconds,
// the report period of other harvests (i.e. non-events) will be the value passed
// in via the reportPeriod paramater.
func getCustomLicenseHarvestTrigger(key collector.LicenseKey, reportPeriod int) HarvestTriggerFunc {
	// The whitelist is injected via make variables in make/secrets.mk
	if whiteListStruct == nil {
		jsonWhiteList := secrets.WhiteList
		err := json.Unmarshal([]byte(jsonWhiteList), &whiteListStruct)
		if err != nil {
			log.Debugf("WARNING: unable to parse whitelist: %v\n", err)
			return nil
		}
	}
	if seconds, ok := whiteListStruct[string(key)]; ok {
		reply := fasterHarvestWhitelistReplyBuilder(seconds)
		return customTriggerBuilder(reply, reportPeriod, time.Second)
	}
	return nil
}

// This function returns the harvest trigger function that should be used for
// this agent.  In priority order:
//   1. Either it uses the ConnectReply to build custom triggers as specified by
//      the New Relic server-side collector.
//   2. Or it creates a default harvest trigger, harvesting all data at the
//      default period.
func getHarvestTrigger(key collector.LicenseKey,
        reply *ConnectReply) HarvestTriggerFunc {
	// First, check the whitelist for faster harvest, passing the the default
	// report period
	trigger := getCustomLicenseHarvestTrigger(key,
	                                          collector.DefaultReportPeriod)

	// If not on the whitelist, build a trigger from the server-side collector
	// configuration.
	if trigger == nil {
		// Build a trigger from the server-side collector configuration.
		if reply.isHarvestAll() {
			trigger = triggerBuilder(HarvestAll,
				time.Duration(collector.DefaultReportPeriod)*time.Second)
		} else {
			trigger = customTriggerBuilder(reply, collector.DefaultReportPeriod,
			                               time.Second)
		}
	}

	// Something in the server-side collector configuration was not well-formed.
	// Build a default HarvestAll trigger.
	if trigger == nil {
		trigger = triggerBuilder(HarvestAll,
			time.Duration(collector.DefaultReportPeriod)*time.Second)
	}

	return trigger
}
