/*
   Source File : PDFDocumentCopyingContext.h


   Copyright 2011 Gal Kahana PDFWriter

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   
*/
#pragma once

#include "EStatusCode.h"
#include "ObjectsBasicTypes.h"
#include "PDFEmbedParameterTypes.h"
#include "PDFDocumentHandler.h"

#include <string>

using namespace std;

class ObjectsContext;
class PDFParser;
class IPDFParserExtender;

namespace PDFHummus
{
	class DocumentContext;
}

using namespace PDFHummus;


class PDFDocumentCopyingContext
{
public:
	PDFDocumentCopyingContext();
	~PDFDocumentCopyingContext(void);

	PDFHummus::EStatusCode Start(const string& inPDFFilePath,
					  DocumentContext* inDocumentContext,
					  ObjectsContext* inObjectsContext,
					  IPDFParserExtender* inParserExtender);

	PDFHummus::EStatusCode Start(IByteReaderWithPosition* inPDFStream,
					  DocumentContext* inDocumentContext,
					  ObjectsContext* inObjectsContext,
					  IPDFParserExtender* inParserExtender);

	EStatusCodeAndObjectIDType CreateFormXObjectFromPDFPage(unsigned long inPageIndex,
															 EPDFPageBox inPageBoxToUseAsFormBox,
															 const double* inTransformationMatrix = NULL);
	EStatusCodeAndObjectIDType CreateFormXObjectFromPDFPage(unsigned long inPageIndex,
															 const PDFRectangle& inCropBox,
															 const double* inTransformationMatrix = NULL);
	EStatusCodeAndObjectIDType AppendPDFPageFromPDF(unsigned long inPageIndex);
	PDFHummus::EStatusCode MergePDFPageToPage(PDFPage* inTargetPage,unsigned long inSourcePageIndex);
	
	EStatusCodeAndObjectIDType CopyObject(ObjectIDType inSourceObjectID);

	/* This one is a bit tricky:
		It copies a direct object, and returns the objects that it references, and still require copying. 
		YOU ARE REQUIRED TO COPY THEM IMMEDIATELY LATER using CopyNewObjectsForDirectObject...otherwise the PDF will be fault.
		The reason for not writing them at point of writing the direct object is simple - 100% of your usages of this
		method it will be in the context of writing something encapsulating this code...you don't want to be disturbed.
		still...there might be referenced objects from this object to copy, hence you must later copy them. The internal
		state of the copying context will hold object references for them till you do, so that the reference IDs remain good.
	*/
	EStatusCodeAndObjectIDTypeList CopyDirectObject(PDFObject* inObject);
	/*
		Call this ONLY with the result of CopyDirectObject, to copy new objects that are reuqired for direct object. It is OK to merge a couple of results from multiple
		CopyDirectObject to a single list. MAKE SURE THERE ARE NO DUPLICATES in that case.
	*/
	PDFHummus::EStatusCode CopyNewObjectsForDirectObject(const ObjectIDTypeList& inReferencedObjects);

	PDFParser* GetSourceDocumentParser();
	IByteReaderWithPosition* GetSourceDocumentStream();
	EStatusCodeAndObjectIDType GetCopiedObjectID(ObjectIDType inSourceObjectID);
	MapIterator<ObjectIDTypeToObjectIDTypeMap> GetCopiedObjectsMappingIterator();

	void End();

	/* Use this method to signal the copying context to avoid copying some objects, but rather use replacements.
		The input map has as keys source object IDs (you can scan for these objects earlier using the parser or some other methods.
		The values in this map are object IDs to use instead. these should be valid IDs in the target PDF, though they do not have to
		be defined yet (can be forward declaration. Note that any keys that are mapped to objects already copied, will be ignored, rather than replaced.
	*/
	void ReplaceSourceObjects(const ObjectIDTypeToObjectIDTypeMap& inSourceObjectsToNewTargetObjects);


	// Used directly, the extendedrs used here will only get CreateFormXObjectsFromPDF and AppendPDFPagesFromPDF events for this
	// copying context. to get full extension of the documetn context, make sure to add the listeners through the document context
	void AddDocumentContextExtender(IDocumentContextExtender* inExtender);
	void RemoveDocumentContextExtender(IDocumentContextExtender* inExtender);	

private:

	PDFHummus::DocumentContext* mDocumentContext;
	PDFDocumentHandler mDocumentHandler;


};
