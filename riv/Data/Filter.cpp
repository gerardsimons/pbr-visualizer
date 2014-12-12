#include "Filter.h"
//namespace riv {
//	size_t Filter::GetId() {
//		return fid;
//	}
//	Filter::Filter() {
//		fid = ++fidCounter;
//	}
//	std::string SingularFilter::GetAttribute() {
//		return attributeName;
//	}
//	SingularFilter::SingularFilter(const std::string& attributeName) {
//		this->attributeName = attributeName;
//	}
//	bool SingularFilter::AppliesToAttribute(const std::string& name) {
//		return attributeName == name;
//	}
//	bool SingularFilter::AppliesToTable(const RIVTable* table) {
//		std::vector<RIVRecord*> records = table->GetRecords();
//		for(RIVRecord* record : records) {
//			if(AppliesToAttribute(record->name)) {
//				return true;
//			}
//		}
//		return false;
//	}
//	std::vector<std::string> SingularFilter::GetAttributes() {
//		std::vector<std::string> attributes(1);
//		attributes[0] = attributeName;
//		return attributes;
//	}
//	RangeFilter::RangeFilter(const std::string& attributeName, float minValue, float maxValue) : SingularFilter(attributeName) {
//		
//		//Normalize
//		if(minValue <= maxValue) {
//			this->minValue = minValue;
//			this->maxValue = maxValue;
//		} else {
//			this->minValue = maxValue;
//			this->maxValue = minValue;
//		}
//	}
//	
//	bool RangeFilter::PassesFilter(const std::string& attribute,float value) {
//		if(attributeName == attribute) {
//			bool passes = (value >= minValue && value <= maxValue);
//			return passes;
//		}
//		return true;
//	}
//
//	bool RangeFilter::PassesFilter(const std::string& attribute,unsigned short value) {
//		if(attributeName == attribute) {
//			return (value >= minValue && value <= maxValue);
//		}
//		return true;
//	}
//	
//	bool RangeFilter::PassesFilter(RIVTable* table, size_t row) {
//		RIVRecord* record = table->GetRecord(attributeName);
//		RIVFloatRecord* floatRecord = RIVTable::CastToFloatRecord(record);
//		if(floatRecord && floatRecord->name == attributeName) {
//			float value = floatRecord->Value(row);
//			return PassesFilter(record->name, value);
//			
//		}
//		RIVUnsignedShortRecord* shortRecord = RIVTable::CastToUnsignedShortRecord(record);
//		if(shortRecord && shortRecord->name == attributeName) {
//			unsigned short value = shortRecord->Value(row);
//			return PassesFilter(record->name, value);
//			
//			
//		}
//		return false;
//	}
//	
//	//Discrete filter stuff
//	DiscreteFilter::DiscreteFilter(const std::string& name, float value) : SingularFilter(name) {
//		this->value = value;
//	}
//	
//	bool DiscreteFilter::PassesFilter(const std::string& name, float value) {
//		return attributeName == name && value == this->value;
//	}
//	bool DiscreteFilter::PassesFilter(const std::string &name, unsigned short value) {
//		return attributeName == name && value == this->value;
//	}
//	
//	//TODO: The constant casting might be costly
//	bool DiscreteFilter::PassesFilter(RIVTable* table, size_t row) {
//		RIVRecord* record = table->GetRecord(attributeName);
//		RIVFloatRecord* floatRecord = RIVTable::CastToFloatRecord(record);
//		if(floatRecord && floatRecord->name == attributeName) {
//			float value = floatRecord->Value(row);
//			if(PassesFilter(record->name, value)) {
//				//What now?
//				return true;
//			}
//		}
//		RIVUnsignedShortRecord* shortRecord = RIVTable::CastToUnsignedShortRecord(record);
//		if(shortRecord && shortRecord->name == attributeName) {
//			unsigned short value = shortRecord->Value(row);
//			if(PassesFilter(record->name, value)) {
//				return true;
//			}
//		}
//		return false;
//	}
//	
//	float DiscreteFilter::GetValue() {
//		return value;
//	}
//	
//	bool CompoundFilter::AppliesToAttribute(const std::string& attributeName) {
//		for(Filter* f : filters) {
//			if(f->AppliesToAttribute(attributeName)) {
//				return true;
//			}
//		}
//		return false;
//	}
//	bool CompoundFilter::AppliesToTable(const RIVTable* table) {
//		for(Filter* f : filters) {
//			if(f->AppliesToTable(table)) {
//				return true;
//			}
//		}
//		return false;
//	}
//	
//	std::vector<Filter*> CompoundFilter::GetFilters() {
//		return filters;
//	}
//	size_t CompoundFilter::Size() {
//		return filters.size();
//	}
//	void CompoundFilter::AddFilter(Filter* newFilter) {
//		if(newFilter != NULL) {
//			filters.push_back(newFilter);
//		}
//	}
//	CompoundFilter::~CompoundFilter() {
//		for(Filter* f : filters) {
//			delete f;
//		}
//	}
//	std::vector<std::string> CompoundFilter::GetAttributes() {
//		std::vector<std::string> allAttributes;
//		for(Filter* f : filters) {
//			std::vector<std::string> ats = f->GetAttributes();
//			AppendAll(allAttributes, ats);
//		}
//		return allAttributes;
//	}
//
////	bool CompoundFilter::AppliesToTable(const RIVTable* table) {
////		std::vector<std::string> tableAttributes = table->GetAttributes();
////		for(std::string filterAttribute : attributes) {
////			bool found = false;
////			for(std::string tableAttribute : tableAttributes) {
////				if(tableAttribute == filterAttribute) {
////					found = true;
////					break;
////				}
////			}
////			if(!found) {
////				return false;
////			}
////		}
////		return true;
////	}
//	ConjunctiveFilter::ConjunctiveFilter(const std::vector<Filter*>& filters) : CompoundFilter(filters) {
//	}
//	bool ConjunctiveFilter::AppliesToTable(const RIVTable* table) {
//		for(Filter* filter : filters) {
//			if(filter->AppliesToTable(table)) {
//				return true;
//			}
//		}
//		return false;
//	}
//	bool ConjunctiveFilter::PassesFilter(const std::string &name, unsigned short value) {
//		for(Filter* filter : filters) {
//			if(filter->AppliesToAttribute(name) && !filter->PassesFilter(name,value)) {
//				return false;
//			}
//		}
//		return true;
//	}
//	bool ConjunctiveFilter::PassesFilter(const std::string &name, float value) {
//		for(Filter* filter : filters) {
//			if(filter->AppliesToAttribute(name) && !filter->PassesFilter(name,value)) {
//				return false;
//			}
//		}
//		return true;
//	}
//	//Check if the given row of the table passes this filter or not by checking all of its filter in AND (conjunctive) logic
//	bool ConjunctiveFilter::PassesFilter(RIVTable* table, size_t row) {
//		for(Filter* filter : filters) {
//			if(filter->PassesFilter(table,row) == false) {
//				return false;
//			}
//		}
//		//It has passed all filters
//		return true;
//	}
//	DisjunctiveFilter::DisjunctiveFilter(const std::vector<Filter*>& filters) : CompoundFilter(filters) {
//
//	}
//
//	
//	bool DisjunctiveFilter::AppliesToTable(const RIVTable* table) {
//		for(Filter* filter : filters) {
//			if(filter->AppliesToTable(table)) {
//				return true;
//			}
//		}
//		return false;
//	}
//	
//	bool DisjunctiveFilter::PassesFilter(const std::string &name, unsigned short value) {
//		for(Filter* filter : filters) {
//			if(filter->AppliesToAttribute(name) && filter->PassesFilter(name,value)) {
//				return true;
//			}
//		}
//		return false;
//	}
//	
//	bool DisjunctiveFilter::PassesFilter(const std::string &name, float value) {
//		for(Filter* filter : filters) {
//			if(filter->AppliesToAttribute(name) && filter->PassesFilter(name,value)) {
//				return true;
//			}
//		}
//		return false;
//	}
//	//Check if the given row of the table passes this filter or not by checking all of its filter in AND (conjunctive) logic
//	bool DisjunctiveFilter::PassesFilter(RIVTable* table, size_t row) {
//		for(Filter* filter : filters) {
//			if(filter->PassesFilter(table,row)) {
//				return true;
//			}
//		}
//		return false;
//	}
//	
//	//Group filter
//	GroupFilter::GroupFilter(Filter* filter, RIVTable* table) : CompoundFilter(filter) {
//		this->sourceTable = table;
//		fetchReferenceRows();
//	}
//	
//	GroupFilter::GroupFilter(const std::vector<Filter*>& filters, RIVTable* sourceTable) : CompoundFilter(filters) {
//		this->sourceTable = sourceTable;
//		fetchReferenceRows();
//	}
//	bool GroupFilter::PassesFilter(const std::string &name, unsigned short value) {
//		for(Filter* filter : filters) {
//			if(filter->AppliesToAttribute(name) && filter->PassesFilter(name,value)) {
//				return true;
//			}
//		}
//		return false;
//	}
//	bool GroupFilter::PassesFilter(const std::string &name, float value) {
//		for(Filter* filter : filters) {
//			if(filter->AppliesToAttribute(name) && filter->PassesFilter(name,value)) {
//				return true;
//			}
//		}
//		return false;
//	}
//	bool GroupFilter::AppliesToTable(const RIVTable* table) {
//		for(Filter* filter : filters) {
//			if(filter->AppliesToTable(table)) {
//				return true;
//			}
//		}
//		return false;
//	}
//	void GroupFilter::fetchReferenceRows() {
//		const std::vector<RIVReference*>* references = sourceTable->GetReferences();
//		for(RIVReference* ref : *references) {
//			//Get the rows of the reference
//			RIVTable* t = ref->targetTable;
//			if(AppliesToTable(t)) {
//				this->ref = ref;
//				return;
//			}
//		}
//	}
//
//	//Check if the given row of the table passes this filter or not by checking all of its filter in AND (conjunctive) logic
//	template<typename... Ts>
//	bool riv::GroupFilter<T>::PassesFilter(RIVTable<Ts...>* table, size_t row) {
//		for(riv::Filter* f : filters) {
//			bool thisFilterPassed = false;
//			std::pair<size_t*,ushort> refRows = ref->GetIndexReferences(row);
////			printArray(refRows.first, refRows.second);
//			if(refRows.first) {
//				for(size_t i = 0 ; i < refRows.second ; ++i) {
//					if(f->PassesFilter(ref->targetTable, refRows.first[i])) {
//						thisFilterPassed = true;
//						break;
//					}
//				}
//			}
//			//The filter was not passed by any reference row
//			if(!thisFilterPassed) return false;
//		}
//		return true;
//	}
//}

//namespace riv {
//	template <class... Ts>
//	bool PassesFilter(RIVTable<Ts...>* table, size_t row);
//	
//	template <class... Ts>
//	bool GroupFilter<Ts...>::PassesFilter(RIVTable<Ts...>* table, size_t row) {
//		for(riv::CompoundFilter<Ts...>* f : CompoundFilter<Ts...>::compoundFilters) {
//			bool thisFilterPassed = false;
//			std::pair<size_t*,ushort> refRows = ref->GetReferenceRows(row);
//	//			printArray(refRows.first, refRows.second);
//			if(refRows.first) {
//				for(size_t i = 0 ; i < refRows.second ; ++i) {
//					if(f->PassesFilter(ref->targetTable, refRows.first[i])) {
//						thisFilterPassed = true;
//						break;
//					}
//				}
//			}
//			//The filter was not passed by any reference row
//			if(!thisFilterPassed) return false;
//		}
//	}
//}