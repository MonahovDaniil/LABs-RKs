# define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>

struct Block {
    int nonce;
    int index;
    std::string data;
    std::string previousHash;
    std::string hash;
    time_t timestamp;

    Block(int idx, const std::string& d, const std::string& prevHash)
        : index(idx), data(d), previousHash(prevHash), timestamp(time(nullptr)), nonce(0) {
        hash = calculateHash();
    }

    std::string calculateHash() const {
        std::string toHash = std::to_string(index) + data + previousHash + std::to_string(timestamp) + std::to_string(nonce);
        return calculateSHA256(toHash);
    }

    std::string calculateSHA256(const std::string& input) const {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);
        std::stringstream ss;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
            ss << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(hash[i]);
        }
        return ss.str();
    }
};

class Blockchain {
private:
    std::vector<Block> chain;
    int difficulty;

    Block createGenesisBlock() {
        return Block(0, "Genesis Block", "0");
    }

public:
    Blockchain(int diff): difficulty(diff) {
        chain.push_back(createGenesisBlock());
    }

    void addBlock(const std::string& data) {
        Block newBlock(chain.size(), data, chain.back().hash);
        newBlock = mineBlock(newBlock);
        const Block& lastBlock = chain.back();
        chain.push_back(newBlock);
    }
    Block mineBlock(Block& block) {
        std::string prefix(difficulty, '0');
        while (block.hash.substr(0, difficulty) != prefix) {
            block.nonce++;
            block.hash = block.calculateHash();
        }
        std::cout << "Block mined: " << block.hash << std::endl;
        return block;
    }

    bool isChainValid() const {
        for (size_t i = 1; i < chain.size(); ++i) {
            const Block& current = chain[i];
            const Block& previous = chain[i - 1];

            if (current.hash != current.calculateHash() || current.previousHash != previous.hash) {
                return false;
            }
        }
        return true;
    }

    void printChain() const {
        for (const auto& block : chain) {
            std::cout << "Block " << block.index << ":\n";
            std::cout << "Data: " << block.data << "\n";
            std::cout << "Hash: " << block.hash << "\n";
            std::cout << "Previous Hash: " << block.previousHash << "\n";
            std::cout << "Timestamp: " << std::ctime(&block.timestamp) << "\n";
            std::cout << "Nonce: " << block.nonce << "\n\n";
        }
    }
};

int main() {
    setlocale(LC_ALL, "rus");
    Blockchain myBlockchain(0);

    while (true) {
        std::cout << "1. Добавить блок\n";
        std::cout << "2. Проверить целостность цепи\n";
        std::cout << "3. Вывести цепь блоков\n";
        std::cout << "4. Сгенерировать блок с уровнем сложности\n";
        std::cout << "5. Выйти\n";
        std::cout << "Выберите действие: ";

        int choice;
        std::cin >> choice;

        if (choice == 1) {
            std::string data;
            std::cout << "Введите данные для блока: ";
            std::cin.ignore();
            std::getline(std::cin, data);
            myBlockchain.addBlock(data);
            std::cout << "Блок добавлен!\n";
        }
        else if (choice == 2) {
            if (myBlockchain.isChainValid()) {
                std::cout << "Цепь блоков действительна.\n";
            }
            else {
                std::cout << "Цепь блоков недействительна!\n";
            }
        }
        else if (choice == 3) {
            myBlockchain.printChain();
        }
        else if (choice == 4) {
            int difficulty;
            std::cout << "Введите уровень сложности (количество нулей в начале хэша): ";
            std::cin >> difficulty;
            Blockchain blockchain(difficulty);
            while (true) {
                std::string data;
                std::cout << "Введите данные для нового блока (или 'exit' для выхода): ";
                std::cin >> data;
                if (data == "exit") {
                    break;
                }
                blockchain.addBlock(data);
                blockchain.printChain();
            }
        }
        else if (choice == 5) {
            std::cout << "Выход...\n";
            break;
        }
        else {
            std::cout << "Неверный выбор. Пожалуйста, попробуйте снова.\n";
        }
    }

    return 0;
}