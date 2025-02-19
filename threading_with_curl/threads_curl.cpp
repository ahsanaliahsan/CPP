#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <mutex>
#include <curl/curl.h>

// Mutex for thread safety
std::mutex download_mutex;

// Helper function to write data into a file
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t total_size = size * nmemb;
    std::ofstream* out_file = static_cast<std::ofstream*>(userp);
    out_file->write(static_cast<char*>(contents), total_size);
    return total_size;
}

// Download a part of the file
void download_part(const std::string& url, int start, int end, std::ofstream& out_file) {
    CURL* curl;
    CURLcode res;
    
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    
    if(curl) {
        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        
        // Set the byte range for downloading
        std::string range = std::to_string(start) + "-" + std::to_string(end);
        curl_easy_setopt(curl, CURLOPT_RANGE, range.c_str());
        
        // Set the write callback to write data to file
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out_file);
        
        // Perform the download
        res = curl_easy_perform(curl);
        
        if(res != CURLE_OK) {
            std::cerr << "Download failed: " << curl_easy_strerror(res) << std::endl;
        }
        
        // Clean up the curl
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

int main() {
    std::string url = "http://example.com/largefile.zip"; // Example URL for a large file
    std::string filename = "downloaded_file.zip";
    
    // Open the file for writing
    std::ofstream out_file(filename, std::ios::binary);
    if (!out_file) {
        std::cerr << "Failed to open file for writing!" << std::endl;
        return 1;
    }
    
    // Assume the file is 10MB and divide it into 5 parts (for simplicity)
    int file_size = 10 * 1024 * 1024; // 10 MB
    int part_size = file_size / 5;
    
    // Create threads to download parts
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        int start = i * part_size;
        int end = (i == 4) ? file_size - 1 : (i + 1) * part_size - 1;
        
        threads.push_back(std::thread(download_part, url, start, end, std::ref(out_file)));
    }
    
    // Wait for all threads to finish
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "File download completed!" << std::endl;
    
    return 0;
}
