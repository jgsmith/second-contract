JSON Parser
===========

The JSON parser provides both encoding and decoding of JSON.

Synopsis
--------

    # include <toollib.h>

    mixed parsed_json;
    string json_serialization;

    parsed_json = JSON_P -> from_json(json_serialization);
    json_serialization = JSON_P -> to_json(parsed_json);

N.B.: This service will serialize only mappings and arrays of values.
