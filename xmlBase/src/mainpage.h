// Mainpage for doxygen

/*! \mainpage package xmlBase
<p>
This package provides services for parsing, interpreting and writing xml.
</p>
<p>
For basic parsing and access to parsed information it depends on the
<a href="http://www.w3.org/TR/DOM-Level-2-Core/">W3C DOM Specification</a>
and in particular makes use of the <a href="http://xml.apache.org/xerces-c/index.html">Xerces implementation</a> of the DOM.  Value added in this package
includes
</p>
<ul>
 <li>several utilities for more convenient retrieval of information from 
     an xml document</li>
 <li>an implementation of the IFile interface, to be used with documents
     such as instrument.xml which have an ini file-like form.  Such
     documents should preferably reference the dtd ifile.dtd in the 
     xmldata subdirectory of this package.  See the file myIFile.xml
     in the same subdirectory for an example of how to do this. </li>
 <li>functions to serialize an in-memory DOM representation of an XML
     document to an output stream.</li>
</ul>
<p>Examples of use of some of these facilities may be found in the
test programs main.cxx, test_mem.cxx and test_IFile.cxx </p>
<hr>
  \section requirements requirements
  \include requirements

*/
