#!/usr/bin/env python

#***************************************************************************
#*                                                                         *
#*   Copyright (c) 2009 Yorik van Havre <yorik@gmx.fr>                     *  
#*                                                                         *
#*   This program is free software; you can redistribute it and/or modify  *
#*   it under the terms of the GNU General Public License (GPL)            *
#*   as published by the Free Software Foundation; either version 2 of     *
#*   the License, or (at your option) any later version.                   *
#*   for detail see the LICENCE text file.                                 *
#*                                                                         *
#*   This program is distributed in the hope that it will be useful,       *
#*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
#*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
#*   GNU Library General Public License for more details.                  *
#*                                                                         *
#*   You should have received a copy of the GNU Library General Public     *
#*   License along with this program; if not, write to the Free Software   *
#*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
#*   USA                                                                   *
#*                                                                         *
#***************************************************************************

__title__="wiki2qhelp"
__author__ = "Yorik van Havre <yorik@gmx.fr>"
__url__ = "http://yorik.orgfree.com"

"""
Caution: work in progress!

This script retrieves the contents of a wiki site and saves it locally,
To be 
"""

#    CONFIGURATION       #################################################

WEBROOT = "http://sourceforge.net/apps/mediawiki/free-cad/index.php?title="
INDEX = "Online_Help_Toc"
TMPFOLDER = "."
NORETRIEVE = ['Manual']

#    END CONFIGURATION      ##############################################


import os, re
from urllib2 import urlopen, HTTPError

css = """/* Basic CSS for offline rendering */

body {
  font-family: Arial,Helvetica,sans-serif;
  font-size: 13px;
  text-align: justify;
  }

h1 {
  font-size: 2.2em;
  font-weight: bold;
  background: #46A4D0;
  color: white;
  padding: 5px;
  -moz-border-radius: 5px;
  -webkit-border-radius: 5px;
  }

a:link, a:visited {
  font-weight: bold;
  text-decoration: none;
  color: #0084FF;
  }

a:hover {
  text-decoration: underline;
  }

.printfooter {
  font-size: 0.8em;
  color: #333333;
  border-top: 1px solid #333333;
  }
"""

def clean(html):
    "cleans given html code from dirty script stuff"
    html = html.replace('\n',' ')
    html = html.replace('\t','')
    html = re.compile('(.*)<div[^>]+column-content+[^>]+>').sub('',html) # stripping before content
    html = re.compile('<div[^>]+column-one+[^>]+>.*').sub('',html) # stripping after content
    html = re.compile('<!--[^>]+-->').sub('',html) # removing comment tags
    html = re.compile('<script[^>]*>.*?</script>').sub('',html) # removing script tags
    html = re.compile('<!--\[if[^>]*>.*?endif\]-->').sub('',html) # removing IE tags
    html = re.compile('<div id="jump-to-nav"[^>]*>.*?</div>').sub('',html) # removing nav div
    html = re.compile('<h3 id="siteSub"[^>]*>.*?</h3>').sub('',html) # removing print subtitle
    return html
    
def output(html,page):
    "encapsulates raw html code into nice html body"
    header = "<html><head>"
    header += "<title>"
    header += page
    header += "</title>"
    header += "<link type='text/css' href='freecad.css' rel='stylesheet'>"
    header += "</head><body>"
    footer = "</body></html>"
    html = header+html+footer
    filename = local(page)
    file = open(filename,'wb')
    file.write(html)
    file.close()

def local(page):
    return TMPFOLDER + '/' + page + '.html'

def getlinks(html):
    "returns a list of wikipage links in html file"
    links = re.findall('<a[^>]*>.*?</a>',html)
    pages = []
    for l in links:
        rg = re.findall('php\?title=(.*)\" title',l)
        if rg: pages.append(rg[0])
    return pages

def cleanlinks(html, pages=None):
    "cleans page links found in html"
    if not pages: pages = getlinks(html)
    for page in pages:
        if  page in NORETRIEVE:
            output = 'href="' + WEBROOT + page + '"'
        else:
            output = 'href="' + page + '.html"'
        html = re.compile('href="[^"]+' + page + '"').sub(output,html)
    return html

def retrieve(page):
    "retrieves given page from the wiki"
    try:
        print "retrieving " + page
        html = (urlopen(WEBROOT + page).read())
        return html
    except HTTPError:
        print 'Error getting page source'

def test(page=None):
    if not page: page = 'Draft_Line'
    html = retrieve(page)
    html = clean(html)
    html = cleanlinks(html)
    output(html,page)
        
