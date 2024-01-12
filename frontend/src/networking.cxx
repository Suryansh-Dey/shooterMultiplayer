#include <curl/curl.h>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append(static_cast<char*>(contents), total_size);
    return total_size;
}
int conn() {
    // Initialize libcurl
    CURL* curl = curl_easy_init();

    if (curl) {
        // Set the URL (replace with any URL you want to test)
        curl_easy_setopt(curl, CURLOPT_URL, "https://jacobsorber.com");

        // Enable SSL certificate verification
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

        // Provide a CA certificate bundle
        curl_easy_setopt(curl, CURLOPT_CAINFO, "curl-ca-bundle.crt");
        // Replace "/path/to/cacert.pem" with the actual path to your CA certificate bundle file.

        // Enable verbose output for more detailed error information
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        // Set the callback function to handle the response
        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Perform the HTTP GET request
        CURLcode res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            // Print the response
            std::cout << "Response:\n" << response << std::endl;
        }

        // Cleanup
        curl_easy_cleanup(curl);
    } else {
        std::cerr << "Failed to initialize libcurl." << std::endl;
        return 1;
    }

    return 0;
}