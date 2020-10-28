# Electra ONE SDK

but electra one publish a json scheme
this can be used to generate classes etc for use in C++ or alike



# Schema 
schema can be found on github, under https://github.com/martinpavlas/electra.one
https://github.com/martinpavlas/electra.one/presets/schema.json


# Developer information can be found at:

https://docs.electra.one/developers/midiimplementation.html


# Code 
the code in this directory is generated using 
https://app.quicktype.io/#l=schema

currently this is using only plain types, so is just the data model
consider using  https://github.com/nlohmann/json

looks like a pretty nice modern alternative to cJSON which Ive used before.
it is also MIT license, and single header file

# Important note

this is currently temporary and for test only as the schema is still subject to change.
therefore for NOW, Ive copied it and edited to get some nice 'starting point' for code generation

