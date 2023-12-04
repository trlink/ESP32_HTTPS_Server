#include "HTTPHeaders.hpp"

namespace httpsserver {

HTTPHeaders::HTTPHeaders() {
  _headers = new std::vector<HTTPHeader *>();
  _mutex   = xSemaphoreCreateMutex();
}

HTTPHeaders::~HTTPHeaders() {
  clearAll();
  delete _headers;

  _headers = NULL;

  vSemaphoreDelete(_mutex);
}

HTTPHeader * HTTPHeaders::get(std::string const &name) {

  xSemaphoreTake(this->_mutex, portMAX_DELAY);

  std::string normalizedName = normalizeHeaderName(name);
  for(std::vector<HTTPHeader*>::iterator header = _headers->begin(); header != _headers->end(); ++header) {
    if ((*header)->_name.compare(normalizedName)==0) {
      xSemaphoreGive(this->_mutex);
      return (*header);
    }
  }

  xSemaphoreGive(this->_mutex);

  return NULL;
}

std::string HTTPHeaders::getValue(std::string const &name) {

  xSemaphoreTake(this->_mutex, portMAX_DELAY);

  std::string normalizedName = normalizeHeaderName(name);
  for(std::vector<HTTPHeader*>::iterator header = _headers->begin(); header != _headers->end(); ++header) {
    if ((*header)->_name.compare(normalizedName)==0) {

      xSemaphoreGive(this->_mutex);

      return ((*header)->_value);
    }
  }

  xSemaphoreGive(this->_mutex);
  return "";
}


void HTTPHeaders::set(HTTPHeader * header) {

  xSemaphoreTake(this->_mutex, portMAX_DELAY);

  for(int i = 0; i < _headers->size(); i++) {
    if ((*_headers)[i]->_name.compare(header->_name)==0) {
      delete (*_headers)[i];
      (*_headers)[i] = header;

      xSemaphoreGive(this->_mutex);

      return;
    }
  }
  _headers->push_back(header);

  xSemaphoreGive(this->_mutex);
}

std::vector<HTTPHeader *> * HTTPHeaders::getAll() {
  return _headers;
}

/**
 * Deletes all headers
 */
void HTTPHeaders::clearAll() 
{
  xSemaphoreTake(this->_mutex, portMAX_DELAY);

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
        };
      } while (_headers->size() > 0);
    };

    _headers->clear();
  };

  xSemaphoreGive(this->_mutex);
}

} /* namespace httpsserver */
