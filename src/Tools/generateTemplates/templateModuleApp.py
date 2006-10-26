#! python
# -*- coding: utf-8 -*-
# (c) 2006 Juergen Riegel 

import template, templateModuleAppMain
import generateBase.generateModel_Module
import generateBase.generateTools

class TemplateModuleApp (template.ModelTemplate):
  def Generate(self):
    AppPath = self.path + "/App/"
    generateBase.generateTools.ensureDir(AppPath)
    
    # the main module files
    AppMain= templateModuleAppMain.TemplateModuleAppMain()
    AppMain.path   = AppPath 
    AppMain.module = self.module
    AppMain.Generate()

    # Features
    if(len(self.module.Content.Feature) != 0):
      generateBase.generateTools.ensureDir(AppPath + "Features")
    print "Genertate() App Dir"

