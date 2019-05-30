/// These are resources floating around the OpenWorld.

#ifndef BEAKER_RESOURCE_H
#define BEAKER_RESOURCE_H

struct BeakerResource {
  size_t resource_type;  // What type of resource is this?
  size_t surface_id;     // Which surface object represents this resource?
};

#endif
