#! python
# -*- coding: utf-8 -*-
# (c) 2006 Juergen Riegel 

import template
import generateBase.generateModel_Module
import generateBase.generateTools

class TemplateFeature (template.ModelTemplate):
	def Generate(self):
		file = open(self.path + self.feature.Name + "Imp.cpp",'w')
		generateBase.generateTools.replace(self.TemplateImplement,locals(),file)
		file = open(self.path + self.feature.Name + ".cpp",'w')
		generateBase.generateTools.replace(self.TemplateModule,locals(),file)
		file = open(self.path + self.feature.Name + ".h",'w')
		generateBase.generateTools.replace(self.TemplateHeader,locals(),file)
		#file.write( generateBase.generateTools.replace(self.Template,locals()))

	TemplateHeader = """
#ifndef __FEATURE@self.module.Name@@self.feature.Name@_H__
#define __FEATURE@self.module.Name@@self.feature.Name@_H__

#include <App/PropertyStandard.h>

#include <App/Feature.h>

namespace @self.module.Name@
{


class @self.feature.Name@ :public App::Feature
{
  PROPERTY_HEADER(@self.module.Name@::@self.feature.Name@);

public:
  @self.feature.Name@();
+ for i in self.feature.Property:
    @i.Type@ @i.Name@;
-

  /** @name methods overide Feature */
  //@{
  /// recalculate the Feature
  virtual int execute(void);
  /// returns the type name of the ViewProvider
  virtual const char* getViewProviderName(void){return "@self.module.Name@Gui::ViewProviderBox";}
  //@}
};

} //namespace @self.module.Name@

#endif // __FEATURE@self.module.Name@@self.feature.Name@_H__

"""
	TemplateModule = """
#include "PreCompiled.h"

#include "@self.feature.Name@.h"
using namespace @self.module.Name@;

PROPERTY_SOURCE(@self.module.Name@::Box, App::Feature)

@self.feature.Name@::@self.feature.Name@()
{
+ for i in self.feature.Property:
  ADD_PROPERTY(@i.Name@,(0.0));
-
}
"""	
	# here the template for the user part of the implementation. This gets NOT overrite when allready exist
	TemplateImplement = """
// 
#include "PreCompiled.h"

#include "@self.feature.Name@.h"
using namespace @self.module.Name@;

// TODO This methode implement the function of the Feature
int @self.feature.Name@::execute(void)
{
   return 0;
}

"""