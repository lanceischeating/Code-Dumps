#include <cstdint>
#include <iostream>
#include <optional>

template <typename key_T, typename value_T>
class HashTable
{
    #define TABLE_SIZE 1000
private:
    bool isDynamic {};
    static constexpr int defaultCapacity = 100;
    int currentCapacity = 0;
    struct HashNode
    {
            key_T key;
            value_T value;
            HashNode* next = nullptr;
            HashNode(key_T  key, const value_T& value, HashNode* n = nullptr) : key(std::move(key)), value(value), next(n) {};
    };
    HashNode* buckets[TABLE_SIZE] = { nullptr };
    HashNode** allocateMap(int32_t capacity = defaultCapacity)
    {
        auto** hashArray = new HashNode*[capacity];
        for (int i = 0; i < capacity; i++)
        {
            HashNode* currentNodePtr = buckets[i];
            while (currentNodePtr)
            {
                HashNode* next = currentNodePtr->next;
            }
        }
        return hashArray;
    }
public:
    HashTable() = default;
    explicit HashTable(bool isDynamic) : isDynamic(isDynamic)
    {
        if (isDynamic == true)
        {

        }
    };


    static uint64_t hash(const key_T& key)
    {
        if constexpr (std::is_integral_v<key_T>)
            return key % TABLE_SIZE;
            
        else if constexpr (std::is_floating_point_v<key_T>)
            return static_cast<uint64_t>(key) * 3 % TABLE_SIZE;
            
        else if constexpr (std::is_same_v<key_T, std::string>)
        {   
            uint64_t returnHash = 1;
            for (unsigned char i : key)
                returnHash *= static_cast<uint64_t>(i) * 32 + 1;
            return returnHash % TABLE_SIZE;   
        }

        return std::hash<key_T>{}(key) % TABLE_SIZE;
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
            this->currentCapacity++;
            return;
        }
        buckets[hashedKey] = newNode;
        this->currentCapacity++;
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
        if (buckets[h_key] != nullptr && buckets[h_key]->key == key) // First link;
        {
            if (buckets[h_key]->next == nullptr)
            {
                value_T temp = buckets[h_key]->value;
                delete buckets[h_key];
                return temp;
            }
            HashNode* tempNode = buckets[h_key];
            value_T tempValue = buckets[h_key]->value;
            buckets[h_key] = buckets[h_key]->next;
            delete tempNode;
            this->currentCapacity--;
            return tempValue;
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
        this->currentCapacity--;
        return std::nullopt;
    }

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
                this->currentCapacity--;
                return true;
            }
            prev = current;
            current = current->next;
        }        
        return false;
    }

    bool isEmpty() { return (this->currentCapacity > 0) ? false : true; }
    int getSize() { return this->currentCapacity; }
    double getCapacity() { return (this->currentCapacity / TABLE_SIZE) * 100; }
    void clear()
    {
        for (int i = 0; i < TABLE_SIZE; i++)
        {
            if (buckets[i])
            {
                HashNode* current = buckets[i];
                HashNode* next = nullptr;
                while (current)
                {
                    next = current->next;
                    delete current;
                    currentCapacity--;
                    current = next;
                }
                buckets[i] = nullptr;
            }
            continue;
        }
    }
    bool replace(const key_T& key, const value_T& value) // Replaces Value for Key if Key and Value exists.
    {
        return true;
    }
    ~HashTable()
    {
        if (isDynamic)
        {
            return;
        }
        for (int i = 0; i < TABLE_SIZE; i++)
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
            continue;
        }
    }
};

int main()
{
    auto* ht = new HashTable<std::string, int>();
    ht->addItem("ABC", 10);
    printf("ABC added to the HashMap \n");
    ht->addItem("BCA", 12);
    printf("BCA added to the HashMap \n");

    printf("Is BCA in the HashMap?%d \n", ht->lookup("BCA"));
    printf("Is ABA in the HashMap?%d \n", ht->lookup("ABA"));
    printf("Is ABC in the HashMap?%d \n", ht->lookup("ABC"));

    const std::optional<int> printValue = ht->retrieveValue("BA");
    printValue ? printf("%d\n", ht->retrieveValue("CA").value()) : printf("No value found\n");
    printf("Checking if HashMap is empty: %d\n", ht->isEmpty());
    ht->clear();
    printf("Clearing HashMap.\n");
    printf("Checking if HashMap is empty: ");
    printf("%d\n", ht->isEmpty());
    return 0;
}
