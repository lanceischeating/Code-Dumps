#include <cstdint>
#include <iostream>
#include <optional>
#include <vector>

template <typename key_T, typename value_T>
class HashTable
{

private:
    static constexpr int defaultCapacity = 100;
    struct HashNode
    {
            key_T key;
            value_T value;
            HashNode* next = nullptr;
            HashNode* prev = nullptr;
            HashNode(key_T  key, const value_T& value, HashNode* n = nullptr) : key(std::move(key)), value(value), next(n) {};
    };

    std::vector<HashNode*> buckets {defaultCapacity, nullptr};

public:
    HashTable() = default;



    uint64_t hash(const key_T& key)
    {
        if constexpr (std::is_integral_v<key_T>)
            return key % buckets.capacity();
            
        else if constexpr (std::is_floating_point_v<key_T>)
            return static_cast<uint64_t>(key) * 3 % buckets.capacity();
            
        else if constexpr (std::is_same_v<key_T, std::string>)
        {   
            uint64_t returnHash = 1;
            for (unsigned char i : key)
                returnHash *= static_cast<uint64_t>(i) * 32 + 1;
            return returnHash % buckets.capacity();
        }

        return std::hash<key_T>{}(key) % buckets.capacity();
    }

    void addItem(const key_T& key, value_T value)
    {
        uint64_t hashedKey = hash(key);
        auto* newNode = new HashNode(key, value);


        if (buckets[hashedKey] != nullptr) // Attach the node to the beginning of the linked list to maintain O(1) insertion.
        {
            HashNode* temp = buckets[hashedKey];
            newNode->next = temp;
            buckets[hashedKey] = newNode;
            return;
        }
        buckets[hashedKey] = newNode;
    }

    bool lookup(const key_T& key)
    {
        uint64_t hashedKey = hash(key); 
        if (buckets[hashedKey] == nullptr) // Does not exist
            return false;
        if (buckets[hashedKey] != nullptr && buckets[hashedKey]->key == key) // First link;
            return true;

        HashNode* temp = buckets[hashedKey];
        
        while (temp != nullptr)
        {
            if (temp->key == key)
                return true;
            temp = temp->next;
        }
        return false;
    }      

    std::optional<value_T> retrieveValue(const key_T& key)
    {
        uint64_t hashedKey = hash(key); 
        if (buckets[hashedKey] == nullptr) // Does not exist
            return std::nullopt;
        if (buckets[hashedKey] != nullptr && buckets[hashedKey]->key == key) // First link;
            return buckets[hashedKey]->value;

        auto* temp = buckets[hashedKey];
        
        while (temp != nullptr)
        {
            if (temp->key == key)
                return temp->value;
            temp = temp->next;
        }

        return std::nullopt;
    }

    std::optional<value_T> popValue(const key_T& key)
    {
        uint64_t h_key = hash(key);
        if (buckets[h_key] == nullptr) // Does not exist
            return std::nullopt;
        if (buckets[h_key]->key == key) // If found on the first link, remove the node and set the head to next;
        {
            value_T returnVal = buckets[h_key]->value;
            HashNode* nextNode = buckets[h_key]->next;
            delete buckets[h_key];
            buckets[h_key] = nextNode;
            return returnVal;
        }

        HashNode* tempNode = buckets[h_key];
        HashNode* prev = nullptr;
        
        while (tempNode != nullptr)
        {
            if (tempNode->key == key)
            {   
                value_T tempValue = tempNode->value;
                if (prev != nullptr)
                    prev->next = tempNode->next;
                else
                    buckets[h_key] = tempNode->next;
                delete tempNode;
                return tempValue;
            }
            prev = tempNode;
            tempNode = tempNode->next;
        }
        return std::nullopt;
    }

    bool isEmpty() { return buckets.empty(); }

    bool deleteEntry(const key_T& key) // Return true on success returns false on failure
    {
        uint64_t h_key = hash(key);
        HashNode* prev = nullptr;
        HashNode* head = buckets[h_key];
        HashNode* current = head;

        while (current != nullptr)
        {
            if (current->key == key)
            {
                if (prev != nullptr)
                    prev->next = current->next;
                else
                    buckets[h_key] = current->next;
                delete current;
                return true;
            }
            prev = current;
            current = current->next;
        }        
        return false;
    }

    void clear()
    {
        for (int i = 0; i < buckets.capacity(); i++)
        {
            if (buckets[i])
            {
                HashNode* current = buckets[i];
                HashNode* next = nullptr;
                while (current)
                {
                    next = current->next;
                    delete current;
                    current = next;
                }
                buckets[i] = nullptr;
            }
        }
    }
    bool replace(const key_T& key, const value_T& value) // Replaces Value for Key if Key and Value exists.
    {
        return true;
    }

    std::tuple<size_t,size_t,size_t>* checks()
    {
        printf("Buckets size: %d\n", buckets.size());
        printf("Buckets capacity: %d\n", buckets.capacity());
        printf("Buckets max_size: %d\n", buckets.max_size());

        return new std::tuple(buckets.size(), buckets.capacity(), buckets.max_size());
    }
    ~HashTable()
    {

        for (int i = 0; i < buckets.size(); i++)
        {
            if (buckets[i])
            {
                HashNode* current = buckets[i];
                HashNode* next = nullptr;
                while (current)
                {
                    next = current->next;
                    delete current;
                    current = next;
                }
            }
        }
    }
};