#pragma once

#include <map>
#include <Item.hpp>
#include <memory>

class ItemFactory { 
public:
    static inline ItemFactory& Get() {
        static ItemFactory itmf;
        return itmf;
    }
        
    void RegisterItem(int id, std::unique_ptr<Item> item) { m_items.insert(std::make_pair(id, std::move(item))); }
    
    template<typename T>
    std::shared_ptr<T> Create(int id) {
        return std::make_shared<T>(dynamic_cast<T&>(*m_items.at(id)));
    }

private:
    std::map<int, std::unique_ptr<Item>> m_items; 
};