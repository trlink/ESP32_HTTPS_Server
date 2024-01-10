#include "HTTPHeaders.hpp"

namespace httpsserver {

HTTPHeaders::HTTPHeaders() {
  _headers = new std::vector<HTTPHeader *>();
  
  #if HTTPHEADERS_USE_MUTEX == 1
    _mutex   = xSemaphoreCreateMutex();
  #endif
}

HTTPHeaders::~HTTPHeaders() {
  clearAll();
  delete _headers;

  _headers = NULL;

  #if HTTPHEADERS_USE_MUTEX == 1
    vSemaphoreDelete(_mutex);
  #endif
}

HTTPHeader * HTTPHeaders::get(std::string const &name) {

  #if HTTPHEADERS_USE_MUTEX == 1
    xSemaphoreTake(_mutex, portMAX_DELAY);
  #endif

  std::string normalizedName = normalizeHeaderName(name);
  for(std::vector<HTTPHeader*>::iterator header = _headers->begin(); header != _headers->end(); ++header) {
    if ((*header)->_name.compare(normalizedName)==0) {
      
      #if HTTPHEADERS_USE_MUTEX == 1
        xSemaphoreGive(_mutex);
      #endif

      
      return (*header);
    }
  }

  #if HTTPHEADERS_USE_MUTEX == 1
    xSemaphoreGive(_mutex);
  #endif

  return NULL;
}

std::string HTTPHeaders::getValue(std::string const &name) {

  #if HTTPHEADERS_USE_MUTEX == 1
    xSemaphoreTake(_mutex, portMAX_DELAY);
  #endif

  std::string normalizedName = normalizeHeaderName(name);
  for(std::vector<HTTPHeader*>::iterator header = _headers->begin(); header != _headers->end(); ++header) {
    if ((*header)->_name.compare(normalizedName)==0) {

      #if HTTPHEADERS_USE_MUTEX == 1
        xSemaphoreGive(_mutex);
      #endif

      return ((*header)->_value);
    }
  }

  #if HTTPHEADERS_USE_MUTEX == 1
    xSemaphoreGive(_mutex);
  #endif

  return "";
}


void HTTPHeaders::set(HTTPHeader * header) {

  #if HTTPHEADERS_USE_MUTEX == 1
    xSemaphoreTake(_mutex, portMAX_DELAY);
  #endif

  for(int i = 0; i < _headers->size(); i++) {
    if ((*_headers)[i]->_name.compare(header->_name)==0) {
      delete (*_headers)[i];
      (*_headers)[i] = header;

      #if HTTPHEADERS_USE_MUTEX == 1
        xSemaphoreGive(_mutex);
      #endif

      return;
    }
  }
  _headers->push_back(header);

  #if HTTPHEADERS_USE_MUTEX == 1
    xSemaphoreGive(_mutex);
  #endif
}

std::vector<HTTPHeader *> * HTTPHeaders::getAll() {
  return _headers;
}

/**
 * Deletes all headers
 */
void HTTPHeaders::clearAll() 
{
  #if HTTPHEADERS_USE_MUTEX == 1
    xSemaphoreTake(_mutex, portMAX_DELAY);
  #endif

  if(_headers != NULL)
  {
    if(_headers->size() > 0)
    {
      do
      {
        if ((*_headers)[0] != NULL) 
        {
          delete (*_headers)[0];

  	  _headers->erase(_headers->begin());
        }
	else
	{
	  HTTPS_LOGE("Err clear all: idx 0 is null");

	  break;
	};
      } while (_headers->size() > 0);
    };

    _headers->clear();
  };

  #if HTTPHEADERS_USE_MUTEX == 1
    xSemaphoreGive(_mutex);
  #endif
}

} /* namespace httpsserver */
