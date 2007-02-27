#! python
# -*- coding: utf-8 -*-
# (c) 2006 Juergen Riegel 

import template
import generateBase.generateModel_Module
import generateBase.generateTools

class TemplateFeature (template.ModelTemplate):
	def Generate(self):
		file = open(self.path + self.feature.Name + ".cpp",'w')
		generateBase.generateTools.replace(self.Template,locals(),file)
		#file.write( generateBase.generateTools.replace(self.Template,locals()))

	Template = """
#include "PreCompiled.h"

#include <Base/Console.h>
#include "@self.feature.Name@.h"


using namespace @self.module.Name@;


PROPERTY_SOURCE(@self.module.Name@::Box, App::Feature)


@self.feature.Name@::@self.feature.Name@()
{
+ for i in self.feature.Property:
  ADD_PROPERTY(@i.Name@,(0.0));
-
}


int@self.feature.Name@::execute(void)
{
   return 0;
}

"""