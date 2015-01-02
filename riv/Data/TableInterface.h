//
//  TableInterface.h
//  embree
//
//  Created by Gerard Simons on 10/12/14.
//
//

#ifndef embree_TableInterface_h
#define embree_TableInterface_h

class RIVReference;

class RIVTableInterface {
public:
	const std::string name;
	RIVReference* reference = NULL;
	//	size_t ID = ++IDCOUNTER;
	
	virtual bool HasRecord(const std::string& name) const = 0;
	virtual void FilterRow(size_t row) = 0;
	virtual size_t NumberOfRows() const = 0;
	virtual void ClearFilteredRows() = 0;
protected:
	RIVTableInterface(const std::string& name) : name(name) {
		
	}
};

#endif
