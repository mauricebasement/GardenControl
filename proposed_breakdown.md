# proposed breakdown:

## introduction
the system is made up of a horizontal-hierarchy. That is, there is main controller, the Brain, and / or 1 or more modules. Each module is able to function on it's own, such that the main controller is essentially a module as well. Each module contains devices (can, may not, doesn't have to), and has varying degrees of autonomy; that is, it may or may not have a clock source, it may or may not provide some functions available from the Brain. Each module should be network-able (ideally with wi-fi), or Bluetooth LE enabled. Serial Port connections should also be possible. All data between modules should be transported in JSON. It is up to the module implementer to handle JSON data structures.

The module should be able to register itself with the Brain, and pending user controlled approval, the Brain will assume configuration and management tasks for the module, so that the user has one entry point into the larger system. Individual modules not registered with the Brain must implement their own configuration interface, while maintaining configuration and management compatibility with the brain.

## Controller (brain)

### General Information
The Brain provides 3 primary functions (2 of which are optional). They consist of:
### General Information
The Brain provides 3 primary functions (2 of which are optional). They consist of:
* Configuration & Management
  * 2 layers/sides(?) - module level, with physical devices contained, and logical level, with real-world location representation.
  * some form of schedule representation
* Resource Monitoring & Automated Response
  * association with module or logical level devices, containers, etc.
  * active components should be able to have triggers for event conditions.
* Historical data store and analysis
  * InfluxDB looks ideal.
  * multiple tags appear to be pretty handy (dev id, location, parent module, etc.)
  * interesting downsampling options, and possibly easy to insert into D3 structures.

#### Configuration & Management
The Brain proposes to manage a set of devices locally, or remotely. It will have a time source (NTP, Radio Clock, Settable RTC), and network access.
The brain should be a central management point for a collection of hardware modules. This includes, but is not limited to, event scheduling, a network management API consisting of a access methods and management based on the class of hardware module, robust error detection and notification, definable active responses for definable event conditions, [and more].

The primary difficulty seems to be the (for me, at least), the device definition, physical access, and access API.

Devices need to be uniquely identifiable, both physically, and logically.
Devices need to be portable.
Device capabilities need to be represented.
Devices need to describe each unit or metric they provide.
Devices need to provide a "driver" method - some form of access including: RESTful resource access, language specific drivers, external binary drivers, etc.
Devices that require arguments must provide error checking, range checking, etc. This write method should also be described (e.g. POST module.local/devices/xxxx/state body: {state: 0} ).

_I will attempt to illustrate the devices I currently have_

```json
{
    "device": "UUID",
    "class": [ "passive"],
    "type": "sensor",
    "metric": [
                {
                    "name": "Temperature",
                    "units": ["Celsius", "Fahrenheit"],
                    "range": { "lower": -55,
                            "upper": 85
                        }
                },
                {
                    "name": "Humidity",
                    "units": [ "percent"],
                    "range": { "lower": 0,
                              "upper": 100
                          }
                }
            ],
    "name": "dht22",
    "RESTaccess": {
        "read":
        [
            {
                "protocol": "http",
                "method": "get",
                "resource": "/devices/uuid/"
            },
            {
                "protocol": "mqtt",
                "method": "pub",
                "resource": "/devices/uuid/read"
            }
        ],
        "write": [
            {
                "protocol": "http",
                "method": "post",
                "resource": "/devices/uuid/"
            },
            {
                "protocol": "mqtt",
                "method": "pub",
                "resource": "/devices/uuid/write"
            }
        ]
    },
    "pinKey": "P9_15"
}
```

Devices, or Modules (collections of devices), will be defined, monitored and updated, so that it will store a full definition of the entire system - including remote modules, storage locations (historical and analytical may be elsewhere), users, logical locations for module groupings, schedules, and recent state histories.

This should be fully represented via JSON.

#### Resource Monitoring & Automated Response
The brain, and/or modules, should be able to monitor resource (device) states/values and set a variety of conditions to be tested. Where possible, automated responses should be available.

Resource Monitoring should define:
* A resource to be read
* A condition to be tested against ( e.g. (LightState === 'on' && AnalogLightSensor > 500) )
* A check frequency (e.g. every 5 mins, twice a day)
* An active or inactive state (e.g. turn monitoring off for this resource)

Automated Response should define:
* resource input interface (access to values)
* response (external program execution like sending mail, sms; internal work with other resources)

#### Historical data store and analysis
InfluxDB provides time series, mongoDB, couchDB, others provide NoSQL JSON stores. It should also be possible to use [RRDtool](http://oss.oetiker.ch/rrdtool/), however it's power lies in charting, and I've not explored it's query capabilities.

Visualization may be done with [D3.js](http://d3js.org), [chartist.js](https://gionkunz.github.io/chartist-js/), or others.

### json requirements
To be clarified: objects should be "append-able", that is, the container should be able to chain meta-data onto them. e.g. a device attached to a smart module might have "local" meta-data, which is only accessible to the smart module. this means that device should have a base representation (exportable), and a detailed representation (internal to implementor). Anything that _imports_ the object should be able to add data - for ex. in the case of the smart module itself, the brain might add, "online: true|false", "lastContact: Date object", etc. Such information may not be relevant to the smart module, but can be relevant to the brain.

Such meta-data can be defined as public and private to the various objects. That is, each object may have it's own private metadata, but represent a "public" sub-set to entities in messages. This may look like, from the device level, a detailed set of data to interact with itself, but it may represent itself to the controller entity as an addressable generic entity, or clients.
_What the Module/Brain sees_
```json
{ "class": "sensor", "type": "temp", "units": "degrees C", "value": 25.0, "_id": "long-unique-id" }
```
_What the device looks like_
```json
{ "class": "sensor", "type": "temp", "units": "degrees C", "_id": "long-unique-id",
  "getTemp": "function(this.pin){ let this.value = value; return value };",
  "pin": "D7",
  "toF": "function(this.value) { return  ((9/5*this.value) + 32 ) };",
  "lastVal": 25.0
  "lastValTime": 1477596705000
}
```
where the "getTemp" & "toF" are methods, and "lastVal" & "lastValTime" are private to the device object.

- uppermost container,
   - contains arrays of objects,
   - should implement dynamic meta data on objects,
     - state
     - last update
     - (optional) previous value(s)
     - (optional) previous value(s) time/date [moment]
   - objects include
     - containers/zones (w/ n-sub containers)
     - devices (some container association must be declared, access methods, data units, etc)
     - schedules (in later.js format),
     - monitored conditions ( the "if", "while", or "for" )
     - all devices (active, passive) will have listable access methods (http, mqtt, local process, and whatever details), upper and/or lower boundaries,
     - condition responses ( the "then" part of the condition" )
     - condition response associations ( so that multiple responses may be defined for a single condition, or a condition can undergo additional processing, based on time, severity, describe status - _Active or Inactive_, etc send mail, sms, turn off, on, get other things involved.
       - executes conditional with associated schedule, listens for emitted message returned from access, emits a message for which a response is listening.  ( e.g. curl -X POST https://modlue.local:2200/device/id/ --json-data '{ pump: { status: on, time: 122330300400, duration: [ 2, "m" ] }', returns some value 200, possibly the full json structure, including connected flow meter rate, any connected valve states, water levels, - potentially a full return of all objects associated with that resource. return responses can be handled like any other message stream.

### REST API definition

this applies much more identifiably to http, but i'm sure its describable via MQTT messages as well, so that the topic is the "resource", but the read, write, update, delete becomes difficult, or at least a little bit more complicated, so that the brain subscribes to a device-topic and publishes to a device-topic, as does the device.

additionally, external data can be accessed via REST, directly from a front end client.

#### message definition
Should Haves:
- object
  - access (r or w)
  - function (possibly chain of functions [ e.g. turn on, get state, dim 30%, get state ])
- more ...

#### HTTP Access
**nomenclature**
- http://brain/  = default front end
- http://module/ = default module reference
- uses CRUD concepts (accessible via `curl -X POST|GET|DELETE -H "Content-Type: application/json" -d '{ varString: "string", varArray: [ 1, 2, 3], varObject: { subVar: "string" }}'`
  - Create = POST, input json, returns json
  - Read = GET, returns json (also HTML5)
  - Update = POST, input json, returns json
  - Delete = DELETE, input ?, returns ?

Example: to get the status of a relay (on or off) from a module, you could use the following, `curl -X GET -H "Content-Type: application/json" http://module/devices/:dev_UUID:` where dev_UUID is a 64bit string identifying the particular device. It should return something like:
```javascript
{
    _id: "2bb91460881b7d5a98418060b0006d66",
    name: "Electrical Outlet 1",
    state: "on",
    watts: 124,
    volts: 230
}
```
#### MQTT Access
