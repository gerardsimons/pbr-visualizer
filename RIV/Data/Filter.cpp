#include "Filter.h"
namespace riv {
	size_t Filter::GetId() {
		return fid;
	}
	Filter::Filter(const std::string& attributeName) {
		attributes.push_back(attributeName);
		fid = ++fidCounter;
	}
	Filter::Filter(const std::vector<std::string>& attributes) {
		this->attributes = attributes;
		fid = ++fidCounter;
	};
	
	bool Filter::AppliesToAttribute(const std::string &name) {
		for(const std::string attribute : attributes) {
			if(name == attribute) {
				return true;
			}
		}
		return false;
	}
	
	bool Filter::AppliesToTable(const RIVTable* table) {
		std::vector<std::string> tableAttributes = table->GetAttributes();
		for(std::string filterAttribute : attributes) {
			bool found = false;
			for(std::string tableAttribute : tableAttributes) {
				if(tableAttribute == filterAttribute) {
					found = true;
					break;
				}
			}
			if(!found) {
				return false;
			}
		}
		return true;
	}
	
	RangeFilter::RangeFilter(const std::string& attributeName, float minValue, float maxValue) : Filter(attributeName) {
		
		//Normalize
		if(minValue <= maxValue) {
			this->minValue = minValue;
			this->maxValue = maxValue;
		} else {
			this->minValue = maxValue;
			this->maxValue = minValue;
		}
	}
	
	bool RangeFilter::PassesFilter(const std::string& attribute,float value) {
		if(attributes[0] == attribute) {
			bool passes = (value >= minValue && value <= maxValue);
			return passes;
		}
		return true;
	}

	bool RangeFilter::PassesFilter(const std::string& attribute,unsigned short value) {
		if(attributes[0] == attribute) {
			return (value >= minValue && value <= maxValue);
		}
		return true;
	}
	
	bool RangeFilter::PassesFilter(RIVTable* table, size_t row) {
		RIVRecord* record = table->GetRecord(attributes[0]);
		RIVFloatRecord* floatRecord = RIVTable::CastToFloatRecord(record);
		if(floatRecord && floatRecord->name == attributes[0]) {
			float value = floatRecord->Value(row);
			return PassesFilter(record->name, value);
			
		}
		RIVUnsignedShortRecord* shortRecord = RIVTable::CastToUnsignedShortRecord(record);
		if(shortRecord && shortRecord->name == attributes[0]) {
			unsigned short value = shortRecord->Value(row);
			return PassesFilter(record->name, value);
			
			
		}
		return false;
	}
	
	//Discrete filter stuff
	DiscreteFilter::DiscreteFilter(const std::string& name, float value) : Filter(name) {
		this->value = value;
	}
	
	bool DiscreteFilter::PassesFilter(const std::string& name, float value) {
		return attributes[0] == name && value == this->value;
	}
	bool DiscreteFilter::PassesFilter(const std::string &name, unsigned short value) {
		return attributes[0] == name && value == this->value;
	}
	
	//TODO: The constant casting might be costly
	bool DiscreteFilter::PassesFilter(RIVTable* table, size_t row) {
		RIVRecord* record = table->GetRecord(attributes[0]);
		RIVFloatRecord* floatRecord = RIVTable::CastToFloatRecord(record);
		if(floatRecord && floatRecord->name == attributes[0]) {
			float value = floatRecord->Value(row);
			if(PassesFilter(record->name, value)) {
				//What now?
				return true;
			}
		}
		RIVUnsignedShortRecord* shortRecord = RIVTable::CastToUnsignedShortRecord(record);
		if(shortRecord && shortRecord->name == attributes[0]) {
			unsigned short value = shortRecord->Value(row);
			if(PassesFilter(record->name, value)) {
				return true;
			}
		}
		return false;
	}
	
	ConjunctiveFilter::~ConjunctiveFilter() {
		for(Filter* f : filters) {
			delete f;
		}
	}
	
	ConjunctiveFilter::ConjunctiveFilter(const std::vector<Filter*>& filters) : Filter("conjunctive") {
		this->filters = filters;
	}
	
	bool ConjunctiveFilter::AppliesToTable(const RIVTable* table) {
		for(Filter* filter : filters) {
			if(filter->AppliesToTable(table)) {
				return true;
			}
		}
		return false;
	}
	
	bool ConjunctiveFilter::PassesFilter(const std::string &name, unsigned short value) {
		for(Filter* filter : filters) {
			if(filter->AppliesToAttribute(name) && !filter->PassesFilter(name,value)) {
				return false;
			}
		}
		return true;
	}
	bool ConjunctiveFilter::PassesFilter(const std::string &name, float value) {
		for(Filter* filter : filters) {
			if(filter->AppliesToAttribute(name) && !filter->PassesFilter(name,value)) {
				return false;
			}
		}
		return true;
	}
	//Check if the given row of the table passes this filter or not by checking all of its filter in AND (conjunctive) logic
	bool ConjunctiveFilter::PassesFilter(RIVTable* table, size_t row) {
		for(Filter* filter : filters) {
			if(filter->PassesFilter(table,row) == false) {
				return false;
			}
		}
		//It has passed all filters
		return true;
	}
	
	DisjunctiveFilter::DisjunctiveFilter(const std::vector<Filter*>& filters) : Filter(AllAttributes()) {
		this->filters = filters;
	}
	
	std::vector<std::string> DisjunctiveFilter::AllAttributes() {
		std::vector<std::string> allAttributes;
		for(Filter* f : filters) {
			std::vector<std::string> ats = f->GetAttributes();
			AppendAll(allAttributes, ats);
		}
		return allAttributes;
	}
	
	bool DisjunctiveFilter::AppliesToTable(const RIVTable* table) {
		for(Filter* filter : filters) {
			if(filter->AppliesToTable(table)) {
				return true;
			}
		}
		return false;
	}
	
	bool DisjunctiveFilter::PassesFilter(const std::string &name, unsigned short value) {
		for(Filter* filter : filters) {
			if(filter->AppliesToAttribute(name) && filter->PassesFilter(name,value)) {
				return true;
			}
		}
		return false;
	}
	
	bool DisjunctiveFilter::PassesFilter(const std::string &name, float value) {
		for(Filter* filter : filters) {
			if(filter->AppliesToAttribute(name) && filter->PassesFilter(name,value)) {
				return true;
			}
		}
		return false;
	}
	//Check if the given row of the table passes this filter or not by checking all of its filter in AND (conjunctive) logic
	bool DisjunctiveFilter::PassesFilter(RIVTable* table, size_t row) {
		for(Filter* filter : filters) {
			if(filter->PassesFilter(table,row)) {
				return true;
			}
		}
		return false;
	}
	
	//Group filter
	GroupFilter::GroupFilter(const std::vector<Filter*>& filters, RIVTable* sourceTable) : Filter("group") {
		this->filters = filters;
		this->sourceTable = sourceTable;
		fetchReferenceRows();
	}
	bool GroupFilter::PassesFilter(const std::string &name, unsigned short value) {
		for(Filter* filter : filters) {
			if(filter->AppliesToAttribute(name) && filter->PassesFilter(name,value)) {
				return true;
			}
		}
		return false;
	}
	bool GroupFilter::PassesFilter(const std::string &name, float value) {
		for(Filter* filter : filters) {
			if(filter->AppliesToAttribute(name) && filter->PassesFilter(name,value)) {
				return true;
			}
		}
		return false;
	}
	bool GroupFilter::AppliesToTable(const RIVTable* table) {
		for(Filter* filter : filters) {
			if(filter->AppliesToTable(table)) {
				return true;
			}
		}
		return false;
	}
	void GroupFilter::fetchReferenceRows() {
		const std::vector<RIVReference*>* references = sourceTable->GetReferences();
		for(RIVReference* ref : *references) {
			//Get the rows of the reference
			RIVTable* t = ref->targetTable;
			if(AppliesToTable(t)) {
				this->ref = ref;
				return;
			}
		}
	}
	//Check if the given row of the table passes this filter or not by checking all of its filter in AND (conjunctive) logic
	bool GroupFilter::PassesFilter(RIVTable* table, size_t row) {
		for(riv::Filter* f : filters) {
			bool thisFilterPassed = false;
			std::pair<size_t*,ushort> refRows = ref->GetIndexReferences(row);
//			printArray(refRows.first, refRows.second);
			if(refRows.first) {
				for(size_t i = 0 ; i < refRows.second ; ++i) {
					if(f->PassesFilter(ref->targetTable, refRows.first[i])) {
						thisFilterPassed = true;
						break;
					}
				}
			}
			//The filter was not passed by any reference row
			if(!thisFilterPassed) return false;
		}
		return true;
	}
	
}