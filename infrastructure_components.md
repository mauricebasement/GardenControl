# Infrastructure Components

* WiFi network
* HTTP stack
* MQTT stack

* Brain will contain:
  * Front End framework
  * back end framework

## front end frameworks:
- [BackBone](https://github.com/jashkenas/backbone)
- Angular - examples for dashboards [Blur](https://github.com/akveo/blur-admin) (I'm not excited about using Ang2.)

## backend frameworks:
- node.js provides http / app logic (daemon)
  - [node red](http://expressjs.com/) OR
  - [express](https://github.com/expressjs/express/)  http://expressjs.com/
- [mosquitto mqtt broker](https://mosquitto.org/) - provides mqtt
- two options: InfluxDB & RRDTool:
  - [influxdata time series DB](https://www.influxdata.com/time-series-platform/influxdb/) provides historical data from sensors, or "logs", good query and structure, fairly light weight
  - [Round Robin Database Tool 'RRDtool'](http://oss.oetiker.ch/rrdtool/) - provides good image file production as charts from stored data. Very light weight, except during image generation, I'm not familiar with query options it provides, not as flexible in access, etc.
  - none of these are recommended for ras-pi or bbb or things with SD card data.

## module framework:
- module dependent
- supports a RESTful API for resource access
- supports MQTT for messaging & alerting
