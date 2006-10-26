#! python
# -*- coding: utf-8 -*-
# (c) 2006 Juergen Riegel 

import template
import generateBase.generateModel_Module
import generateBase.generateTools

class TemplateModuleAppMain (template.ModelTemplate):
  def Generate(self):
    generateBase.generateTools.ensureDir(self.path)
    print "Genertate() App Dir"

