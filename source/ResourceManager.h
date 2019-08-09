/// This is the world for BeakerOrgs
#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

///< Includes from Empirical
#include "Evolve/World.h"
#include "geometry/Surface.h"
#include "geometry/Point2D.h"
#include "base/assert.h"

///< Experiment headers
#include "config.h"
#include "BeakerResource.h"

///< Managing resources directly
using man_t = emp::vector<BeakerResource>;  
///< Keeping tabs on resources (consumed or not)
using tab_t = emp::vector<bool>; 


class ResourceManager
{
	private:

		BeakerConfig & config;      ///< BeakerConfig for possible values

		man_t manager;			    		///< Structure to hold all resources for a given sector
		tab_t tabs;									///< Structure to tell if a resource has been eaten or not


	public:

		/* Constructors, Destructors, and Operators */

		ResourceManager(BeakerConfig & _config) : config(_config)
		{
			emp_assert(config.RESOURCE_UPS_MAX() > 0, config.RESOURCE_UPS_MAX());
			PopulateManagers();
		}

		~ResourceManager()
		{
			manager.clear();
			tabs.clear();
		}

		const BeakerResource & operator[](size_t id);


		/* Functions dedicated to Initalization */

		void PopulateManagers(); 					///< Will populate the Manager and configure the resources


		/* Functions dedicated to maintain manager */

		void Consumed(size_t mid);									///< Set resource as been consumed 
		bool Alive(size_t mid);											///< Is the resource still alive? 
		void Reset();																///< Reset everything to not consumed (false)
		bool Expired(size_t mid);										///< Has a specific resource expired? 
		void Increment();														///< Increment all resource update count


		/* Getter and setter functions */

		const man_t GetMana() const { return manager; };			///< Get class manager containers
		const tab_t GetTabs() const { return  tabs; };

		size_t GetMapID(size_t pos);													///< Get resource variables
		size_t GetSurfaceID(size_t pos);

		BeakerResource & GetRes(size_t mid);									///< Get reference to  resource

		void SetMapID(size_t pos, size_t mid);								///< Set resoruce variables
		void SetSurfaceID(size_t pos, size_t sid);



		/* Functions dedicated to debugging */

		void PrintManager();


		/* Functions dedicated to */
};

/* Constructors, Destructors, and Operators */

const BeakerResource & ResourceManager::operator[](size_t id)
{
	emp_assert(id > 0, id);
	emp_assert(id < manager.size(), id);

	return manager[id];
}


/* Functions dedicated to Initalization */

void ResourceManager::PopulateManagers()
{
	//< Set up the number of sectors for x,y axis
	manager.resize(config.NUMBER_RESOURCES());
	tabs.resize(config.NUMBER_RESOURCES(), false);

	//< Populate the Manager
	for(size_t i = 0; i < config.NUMBER_RESOURCES(); ++i)
	{
		//< Grab appropiate resource and configure it
		BeakerResource & res = manager[i];

		//< Config resource!
		res.SetInit(true);
		res.SetMapID(i);
	}

	//< Make sure all resources have been initialized in the vector
	if(config.TESTING())
	{
		for(size_t i = 0; i < manager.size(); ++i)
		{
			emp_assert(manager[i].GetInit() == true, manager[i].GetInit());
		}
	}
	
}


/* Functions dedicated to maintain manager */

bool ResourceManager::Alive(size_t mid) 					///< Has the resource been eaten yet?
{
	emp_assert(mid < tabs.size(), mid);
	emp_assert(tabs[mid].GetInit() == true, tabs[mid].GetInit());
	return tabs[mid];
}

void ResourceManager::Consumed(size_t mid)				///< Resource has been consumed
{
	emp_assert(mid < tabs.size(), mid);
	emp_assert(tabs[mid].GetInit() == true, tabs[mid].GetInit());
	tabs[mid] = true;
}

void ResourceManager::Reset()										///< Reset everything to not consumed
{
	tabs.clear();
	tabs.resize(config.NUMBER_RESOURCES(), false);
}

bool ResourceManager::Expired(size_t mid)
{
	emp_assert(mid > 0, mid);
	emp_assert(mid < manager.size(), mid);
	
	return manager[mid].Expired(config.RESOURCE_UPS_MAX());
} 

void ResourceManager::Increment()
{
	for(size_t i = 0; i < manager.size(); ++i)
	{
		
	}
}


/* Getter and setter functions */

BeakerResource & ResourceManager::GetRes(size_t mid)
{
	emp_assert(mid > 0; mid);
	emp_assert(mid < config.NUMBER_RESOURCES(), mid);
	return manager[mid];
}

size_t ResourceManager::GetMapID(size_t pos)													///< Get resource variables
{
	emp_assert(pos > 0; pos);
	emp_assert(pos < config.NUMBER_RESOURCES(), pos);

	return manager[pos].GetMapID();
}

size_t ResourceManager::GetSurfaceID(size_t pos)
{
	emp_assert(pos > 0; pos);
	emp_assert(pos < config.NUMBER_RESOURCES(), pos);

	return manager[pos].GetSurfaceID();
}

void ResourceManager::SetMapID(size_t pos, size_t mid)
{
	emp_assert(pos > 0, pos); 
	emp_assert(pos < manager.size(), pos);
	emp_assert(mid > 0, mid);

	manager[pos].SetMapID(mid);
}

void ResourceManager::SetSurfaceID(size_t pos, size_t sid)
{
	emp_assert(pos > 0, pos); 
	emp_assert(pos < manager.size(), pos);
	emp_assert(sid > 0, sid);

	manager[pos].SetSurfaceID(sid);
}


/* Functions dedicated to debugging */

void ResourceManager::PrintManager()
{
	for(size_t i = 0; i < manager.size(); ++i)
	{
		std::cout << "i=" << i << std::endl;
		manager[i].PrintStats();
		std::cout << std::endl;
	}
}


#endif