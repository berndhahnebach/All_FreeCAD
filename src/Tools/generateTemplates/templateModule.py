#! python
# -*- coding: utf-8 -*-
# (c) 2006 Juergen Riegel 

import template, templateModuleApp,templateModuleGui
import generateBase.generateModel_Module

class TemplateModule (template.ModelTemplate):
    def Generate():
        print "generateBase.generateModel_Module.Genertate()\n"
        App= generateTemplates.templateModuleApp.TemplateModuleApp()
        App.path = head + "/App"
        App.module = GenerateModelInst.Module
        App.Generate()

        

