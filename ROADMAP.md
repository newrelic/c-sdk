# C SDK Roadmap

## End of Life Notice ##
As of April 2022, we're discontinuing support for several capabilities, including the C SDK. Please explore the [option to use OpenTelemetry](https://github.com/newrelic/docs-website/blob/develop/docs/apm/agents/c-sdk/get-started/otel_cpp_example) as an alternative to send C++ telemetry data to New Relic. For more details about the EOL, see our [Explorers Hub post](https://discuss.newrelic.com/t/q1-bulk-eol-announcement-fy23/181744).

## Product Vision
The goal of the C SDK is to provide visibility into the health of your service. The agent provides metrics about the runtime health of your service and the process it runs in, and traces that show how specific requests are performing. It also provides information about the environment in which it is running, so you can identify issues with specific hosts, regions, deployments, and other facets. 

New Relic is moving toward OpenTelemetry. OpenTelemetry is a unified standard for service instrumentation. You will soon see a new repository for a New Relic OpenTelemetry Exporter that will allow you to use the CPP OpenTelemetry SDK and send data up to New Relic. OpenTelemetry will include a broad set of high-quality community-contributed instrumentation and a powerful vendor-neutral API for adding your own instrumentation.

