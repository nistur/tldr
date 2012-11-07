#include "tldr.h"

#include <string.h>

#ifndef  TLDR_MAX_NODES
# define TLDR_MAX_NODES 256
#endif /*TLDR_MAX_NODES*/

#ifndef  TLDR_MAX_DEPTH
# define TLDR_MAX_DEPTH 128
#endif /*TLDR_MAX_DEPTH*/

struct _node
{
    char    name[TLDR_STRING_MAX];
    char    path[TLDR_STRING_MAX];
    _node*  children[TLDR_MAX_NODES];
    _node*  parent;

    bool    hasValue; 
    char    value[TLDR_STRING_MAX];
};

void tldrInitNode(_node* node, _node* parent)
{
    *node->name = 0;
    *node->path = 0;
    for(int i=0; i<TLDR_MAX_NODES; ++i)
	node->children[i] = 0;
    node->parent = parent;
    node->hasValue = false;
    *node->value = 0;
}

void tldrTerminateNode(_node** node);
void tldrClearNode(_node* node)
{
    for(int i=0; i<TLDR_MAX_NODES; ++i)
	tldrTerminateNode(&node->children[i]);
    // reset to empty node
    tldrInitNode(node, node->parent);
}

void tldrTerminateNode(_node** node)
{
    if(!*node)
	return;
    tldrClearNode(*node);
    delete *node;
    *node = 0;    
}

inline tldrReturn tldrToken(const char* str, char* tok)
{
    static const char* s_string = 0;
    static const char* s_pStart = 0;
    if(s_string == 0 && str == 0) return TLDR_NO_CONTEXT;
    if(str == 0 && tok == 0) { s_string = s_pStart = 0; return TLDR_NO_CONTEXT; }

    if(str && str != s_string) s_string = s_pStart = str;
    if(!*s_pStart) return TLDR_NO_CONTEXT;

    const char* pEnd = s_pStart;
    while(*pEnd != '.' && *pEnd != 0) pEnd++;
    int len = pEnd - s_pStart;
    memcpy(tok, s_pStart, len);
    tok[len] = 0;
    s_pStart = *pEnd ? pEnd + 1 : pEnd;

    return TLDR_SUCCESS;
}

struct tldrContext
{
    _node   root;
    _node*  values[TLDR_MAX_NODES];
};

void tldrClearContext(tldrContext* context)
{
    if(!context)
	return;
    for(int i=0;i<TLDR_MAX_NODES;++i)
	context->values[i] = 0;
    tldrInitNode(&context->root, 0);
}

tldrContext* tldrInitContext()
{
    tldrContext* context = new tldrContext;
    tldrClearContext(context);
    return context;
}

tldrReturn tldrTerminateContext(tldrContext** context)
{
    if(!*context)
	return TLDR_NO_CONTEXT;
    delete *context;
    *context = 0;
    return TLDR_SUCCESS;
}

_node* tldrGetNode(tldrContext* context, const char* key, bool create)
{
    if(!context)
	return 0;
    const char* pStart = key;
    const char* pEnd   = key;
    
    _node* node = &context->root;

    char path[TLDR_STRING_MAX];
    *path = 0;
    char* pPath = path;
    while(*pStart != 0 && pStart != key + TLDR_STRING_MAX)
    {
	while(*pStart == '.') pStart++;
	pEnd = pStart;

	while(*pEnd != '.' && *pEnd != 0) pEnd++;
	// between pStart and pEnd, we should have the next node
	int len = pEnd - pStart;
	char* name = new char[len + 1];
	memcpy(name, pStart, len);
	name[len] = 0;

	// update the current path
	if(pPath != path)
	{
	    *pPath = '.';
	    pPath++;
	}
	memcpy(pPath, name, len);
	pPath += len;
	*pPath = 0;
	
	_node* next = node;
	for(int i=0; i< TLDR_MAX_NODES; ++i)
	    if(node->children[i] && 
	       node->children[i]->name[0] == name[0] &&
	       strcmp(node->children[i]->name, name) == 0)
	    {
		next = node->children[i];
		break;
	    }
	if(next == node && create)
	    for(int i=0; i< TLDR_MAX_NODES; ++i)
		if(!node->children[i])
		{
		    next = new _node;
		    tldrClearNode(next);
		    memcpy(next->name, name, len + 1);
		    memcpy(next->path, path, TLDR_STRING_MAX);
		    node->children[i] = next;
		    break;
		}
	if(next == node)
	{
	    delete [] name;
	    return 0;
	}
	node = next;

	delete [] name;
	pStart = pEnd;
    }

    return node;
}

tldrReturn tldrWriteValue(tldrContext* context, const char* key, const char* val)
{
    if(!context)
	return TLDR_NO_CONTEXT;
    
    _node* node = tldrGetNode(context, key, true);
    if(!node)
	return TLDR_NO_MEMORY;

    memcpy(node->value, val, TLDR_STRING_MAX);
    node->hasValue = true;
    
    for(int i=0;i<TLDR_MAX_NODES;++i)
	if(context->values[i] == node)
	    return TLDR_SUCCESS;
    
    for(int i=0;i<TLDR_MAX_NODES;++i)
	if(!context->values[i])
	{
	    context->values[i] = node;
	    return TLDR_SUCCESS;
	}
    return TLDR_NO_MEMORY;
}

tldrReturn tldrReadValue(tldrContext* context, const char* key, char* val)
{
    if(!context)
	return TLDR_NO_CONTEXT;

    _node* node = tldrGetNode(context, key, false);
    if(!node)
	return TLDR_NO_KEY;

    if(!node->hasValue)
	return TLDR_NO_VALUE;
    
    memcpy(val, node->value, TLDR_STRING_MAX);
    return TLDR_SUCCESS;
}

tldrReturn tldrKeyExists(tldrContext* context, const char* key)
{
    if(!context)
	return TLDR_NO_CONTEXT;
    
    _node* node = tldrGetNode(context, key, false);
    if(!node)
	return TLDR_NO_KEY;
    if(!node->hasValue)
	return TLDR_NO_VALUE;
    return TLDR_SUCCESS;
}

#include <stdio.h>
struct _header
{
    char id[4];
    int  pathofs;
    int  valofs;
    char npaths;
};
const char* ID = "TLDR";

tldrReturn tldrSaveBinary(tldrContext* context, const char* filename)
{
    if(!context)
	return TLDR_NO_CONTEXT;

    // absolute maximum size we could be saving
    long size = TLDR_STRING_MAX * TLDR_MAX_NODES * 2;
    char buffer[size];
    tldrReturn res = tldrGenerateBinary(context, buffer, size);
    if(res != TLDR_SUCCESS) return res;

    FILE* fp = fopen(filename, "wb");
    if(!fp)
	return TLDR_WRITE_PROTECT;
    fwrite(buffer, 1, size, fp);

    fclose(fp);
    return TLDR_SUCCESS;
}

tldrReturn tldrLoadBinary(tldrContext* context, const char* filename, bool clear)
{
    if(!context)
	return TLDR_NO_CONTEXT;

    if(clear)
	tldrClearContext(context);
    
    FILE* fp = fopen(filename, "rb");
    if(!fp)
	return TLDR_NO_FILE;

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    char* buffer = new char[size];
    fread(buffer, 1, size, fp);

    tldrReturn res = tldrParseBinary(context, buffer, size);
    
    delete [] buffer;

    fclose(fp);
    return res;
}

struct _val
{
    int pathofs;
    int valofs;
    int nextofs;
};

tldrReturn tldrGenerateBinary(tldrContext* context, char* buffer, long& size)
{
    if(!context)
	return TLDR_NO_CONTEXT;

    long currentSize = 0;

    // first off, set the header
    _header* header = (_header*)buffer;
    mempcpy(header->id, ID, 4);
    header->pathofs = sizeof(_header);
    currentSize += header->pathofs;

    char* pBuf = buffer + currentSize;

    // right now we can only have sequential nodes
    // but it should be possible to remove nodes
    // so there's no guarantee they will always be
    // stored sequentially in context->values
    // so copy them to a local array first
    _node* values[TLDR_MAX_NODES];
    int cnt = 0;
    for(int i=0; i<TLDR_MAX_NODES; ++i)
	if(context->values[i])
	    values[cnt++] = context->values[i];

    char paths[TLDR_MAX_NODES][TLDR_STRING_MAX];

    char tok[TLDR_STRING_MAX];
    int nPaths = 0;
    for(int i=0; i<cnt; ++i)
    {
	while(tldrToken(values[i]->path, tok) == TLDR_SUCCESS)
	{ 
	    bool exists = false;
	    for(int i=0; i<nPaths; ++i)
		if(strcmp(tok, paths[i]) == 0)
		{
		    exists = true;
		    break;
		}
	    if(exists) continue;
	    
	    int len = strlen(tok) + 1;
	    if(currentSize + len > size) return TLDR_NO_MEMORY;
	    memcpy(paths[nPaths], tok, len);
	    memcpy(pBuf, tok, len);
	    currentSize += len;
	    pBuf = buffer + currentSize;
	    nPaths++;
	}
    }
    tldrToken(0,0);

    header->npaths = nPaths;
    header->valofs = currentSize;

    for(int i=0; i<cnt; ++i)
    {
	_val* val = (_val*)pBuf;
	val->pathofs = sizeof(_val);
	char pathIDs[TLDR_MAX_NODES];
	int ntok = 0;
	pBuf += val->pathofs;
	while(tldrToken(values[i]->path, tok) == TLDR_SUCCESS)
	    for(int i=0; i<nPaths; ++i)
		if(strcmp(tok, paths[i]) == 0)
		{
		    pathIDs[ntok++] = i+1;
		    break;
		}
	pathIDs[ntok++] = 0;
	tldrToken(0,0);
	memcpy(pBuf, pathIDs, ntok);
	val->valofs  = val->pathofs + ntok; 
	pBuf += ntok;
	int len = strlen(values[i]->value) + 1;
	memcpy(pBuf, values[i]->value, len);
	pBuf += len;
	currentSize = pBuf - buffer;
	val->nextofs = (i < cnt-1) ? pBuf - (char*)val :0;
    }
    
    size = currentSize;
    return TLDR_SUCCESS;
}
 
tldrReturn tldrParseBinary(tldrContext* context, const char* buffer, long size)
{
    if(!context)
	return TLDR_NO_CONTEXT;

    _header* header = (_header*)buffer;
    if(memcmp(header->id, ID, 4) != 0)
	return TLDR_INVALID_HEADER;

    char paths[TLDR_MAX_NODES][TLDR_STRING_MAX];

    const char* path = buffer + header->pathofs;
    for(int i=0; i<header->npaths; ++i)
    {
	int len = strlen(path);
	sprintf(paths[i], "%s", path);
	path += len + 1;
    }

    _val* val = (_val*)(buffer + header->valofs);
    while(val)
    {
	char key[TLDR_STRING_MAX];
	*key = 0;
	char* pathID = (char*)val + val->pathofs;
	while(*pathID)
	{
	    int id = *pathID - 1;
	    int len = strlen(key);
	    if(!len)
		sprintf(key, "%s", paths[id]);
	    else
		sprintf(key + len, ".%s", paths[id]);
	    pathID++;
	}
	
	const char* value = (char*)val + val->valofs;
	tldrWriteValue(context, key, value);

	val = val->nextofs ? val + val->nextofs : 0;
    }

    return TLDR_SUCCESS;
}
