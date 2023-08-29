#include <iostream>
#include <filesystem>
#include <list>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <string>

#include "macros.cpp"


int log_cleaner(char **argv);

void list_dirs(const std::filesystem::path &dir_path, std::list<std::filesystem::path> *dir_list);

void list_files(const std::filesystem::path &dir_path, std::list<std::filesystem::path> &file_list);

bool is_sign(std::string &sign);

int main(int argc, char **argv) {

    /* introduce the utility */
    std::cout << END_LINE << SPACE << INTRO << END_LINE << END_LINE;

    /* check input parameter */
    std::cout << PRC_CHECKING << END_LINE;
    if ((argc < THREE) || (argv[ONE] == nullptr) ||
        (argv[TWO] == nullptr) || (argv[THREE] == nullptr)) {
        std::cerr << SPACE << ERR_MSG << END_LINE;
        std::cout << SPACE << HELP_MSG << END_LINE << END_LINE;
        exit(MINUS_ONE);
    }

    /* initialization */
    std::cout << PRC_INIT << END_LINE;
    bool is_in_collect{true};
    std::filesystem::path base_dir_path{argv[ONE]};
    auto *fn = new std::string{};
    auto *index = new size_t{};
    auto *line_counter = new size_t{0};
    auto *result_file = new std::string{argv[TWO]};
    auto *words_file_name = new std::string{argv[THREE]};
    auto *dirs = new std::list<std::filesystem::path>{};
    auto *file_list = new std::list<std::filesystem::path>{};
    auto *word_list = new std::list<std::string>{};
    auto *word_and_list = new std::list<std::string>{};
    auto *words_file = new std::ifstream{};
    auto *input_file = new std::ifstream{};
    auto *output_file = new std::ofstream{};
    auto *line = new std::string{};

    /* initial word list to search */
    std::cout << PRC_CHK_WRD << END_LINE;
    words_file->open(words_file_name->c_str(), std::ios::in);
    while (std::getline(*words_file, *line)) {
        if (!line->starts_with(SHARP))
            word_list->emplace_back(*line);
    }
    words_file->close();
    line->clear();

    if (word_list->empty()) {
        std::cout << SPACE << No_WORD << END_LINE;
        std::cout << SPACE << HELP_MSG << END_LINE << END_LINE;
        exit(MINUS_ONE);
    }

    /* read list of file directory and put them into the file_list collection */
    std::cout << PRC_GTH << END_LINE;
    dirs->emplace_back(base_dir_path);
    list_dirs(base_dir_path, dirs);
    for (std::filesystem::path &dir: *dirs) {
        list_files(dir, *file_list);
    }

    /* open result file */
    if (output_file->is_open()) output_file->close();
    output_file->open(result_file->c_str(), std::ios::out | std::ios::app);
    if (!output_file->is_open()) {
        std::cout << ERR_OPEN_FILE << END_LINE;
        exit(MINUS_ONE);
    }

    /* write xml header file and open tag into result file */
    *output_file << SIGN << END_LINE;
    *output_file << XML_HEADER << END_LINE << END_LINE << LOG_OPEN_TAG << END_LINE;

    /* --------------------------------------------------------- */

    /* reading all log files */
    std::cout << PRC_START << END_LINE << END_LINE;
    for (std::filesystem::path &file_name: *file_list) {
        *index += 1;
        fn->clear();
        *fn = file_name.generic_string();

        /* write file name to console */
        std::cout << SPC4 << *index << SP_DASH_SP << *fn << END_LINE;

        /* write file name to the result file */
        *output_file << END_LINE << SPC4 << *index << SP_DASH_SP << *fn << END_LINE;
        *output_file << SPC4 << SEPARATOR << std::endl;

        if (input_file->is_open()) input_file->close();
        input_file->open(*fn, std::ios::in);
        *line_counter = ZERO;

        /* search in all lines */
        while (std::getline(*input_file, *line)) {
            if(is_sign(*line)) break;
            *line_counter += 1;
            boost::trim(*line);
            if (line->empty()) continue;
            if (!line->starts_with(GRATE)) {
                *output_file << *line_counter << SP_DASH_SP << *line << END_LINE;
                continue;
            }
            for (std::string &word: *word_list) {
                boost::trim(word);
                if (word.empty()) continue;
                if (word.contains(DELIM)) {
                    boost::split(*word_and_list, word, boost::is_any_of(DELIM),
                                 boost::token_compress_on);
                    for (std::string &and_word: *word_and_list) {
                        is_in_collect = true;
                        boost::trim(and_word);
                        if (!line->contains(and_word)) {
                            is_in_collect = false;
                            break;
                        }
                    }
                    if (is_in_collect) {
                        *output_file << SPC6 << *line_counter << SP_DASH_SP << *line << END_LINE;
                    }
                } else if (line->contains(word)) {
                    *output_file << SPC6 << *line_counter << SP_DASH_SP << *line << END_LINE;
                }
            }
        }

        /* close current input file */
        input_file->close();
    }

    *output_file << END_LINE << LOG_CLOSE_TAG << END_LINE;

    if (input_file->is_open()) input_file->close();
    if (output_file->is_open()) output_file->close();

    delete index;
    delete fn;
    delete line_counter;
    delete result_file;
    delete words_file_name;
    delete dirs;
    delete file_list;
    delete word_list;
    delete word_and_list;
    delete words_file;
    delete input_file;
    delete output_file;
    delete line;

    std::cout << END_LINE << PRC_CLEAN << END_LINE;
    log_cleaner(argv);

    std::cout << END_LINE << SPACE << END_MSG << END_LINE << std::endl;
    return EXIT_SUCCESS;
}


/* ----------------------- */

int log_cleaner(char **argv) {

    auto *clear_lines = new std::list<std::string>{};
    auto *clear_line = new std::string{};

    auto *read_file = new std::ifstream{};
    auto *write_file = new std::ofstream{};

    try {

        /* open result file */
        read_file->open(argv[TWO], std::ios::in);

        /* open clean result file */
        write_file->open(std::string(argv[TWO]) + CLEAN_EXT, std::ios::out);

        /* -------------------------------------------- */

        /* read all lines of result file */
        while (getline(*read_file, *clear_line)) {

            /* trim current  line */
            boost::trim(*clear_line);

            /* if line is blank ignore it */
            if (clear_line->length() == ZERO || clear_line->starts_with(END_LINE))
                continue;

            /* specified line title */
            if (clear_line->ends_with(TXT) ||
                clear_line->ends_with(LOG) ||
                clear_line->ends_with(XML) ||
                clear_line->starts_with(THREE_MAD)) {

                if (clear_lines->size() < TWO) {
                    clear_lines->push_back(*clear_line);
                    continue;

                } else {
                    clear_lines->clear();
                    clear_lines->push_back(*clear_line);
                    clear_line->clear();
                    continue;
                }
            } else {
                if (clear_lines->size() == TWO && !clear_line->starts_with(LOG_CLOSE_TAG)) {
                    *write_file << END_LINE;
                    for (std::string &line_str: *clear_lines) {
                        *write_file << SPC4 << line_str << END_LINE;
                    }
                    *write_file << SPC6 << *clear_line << D_END_LINE;
                    clear_lines->clear();
                    clear_line->clear();
                } else if (clear_lines->size() == ONE) {
                    *write_file << ERR_INV_FMT << END_LINE;
                    clear_lines->clear();
                    clear_line->clear();
                } else {
                    if (clear_line->starts_with(GRATE)) {
                        *write_file << *clear_line << D_END_LINE;
                    } else {
                        *write_file << SPC6 << *clear_line << D_END_LINE;
                    }
                    clear_lines->clear();
                    clear_line->clear();
                }
            }
        }
        *write_file << std::endl;
    }
    catch (const std::exception &e) {
        std::cerr << SPACE << e.what() << D_END_LINE;
        exit(MINUS_ONE);
    }

    read_file->close();
    write_file->close();

    delete clear_lines;
    delete clear_line;
    delete read_file;
    delete write_file;

    return EXIT_SUCCESS;
}

/* ----------------------- */
void list_dirs(const std::filesystem::path &dir_path, std::list<std::filesystem::path> *dir_list) {
    std::filesystem::directory_iterator result =
            std::filesystem::directory_iterator(dir_path);
    for (const std::filesystem::directory_entry &de: result) {
        if (de.is_directory()) {
            dir_list->emplace_back(de.path());
            list_dirs(de.path(), dir_list);
        }
    }
}

/* ----------------------- */

void list_files(const std::filesystem::path &dir_path, std::list<std::filesystem::path> &file_list) {
    std::filesystem::directory_iterator result =
            std::filesystem::directory_iterator(dir_path);
    for (const std::filesystem::directory_entry &de: result) {
        std::string file_name = de.path().generic_string();
        bool rf = de.is_regular_file();
        bool xml = file_name.ends_with(XML);
        bool log = file_name.ends_with(LOG);
        bool txt = file_name.ends_with(TXT);
        if (rf && (xml || log || txt)) {
            file_list.emplace_back(de.path());
        }
    }
}

/* ----------------------- */

bool is_sign(std::string &sign) {
    size_t index = ZERO;
    const char s[] = CHECK_SIGN;
    for (char ch: sign) {
        if (isspace(ch)) continue;;
        if (ch != tolower(s[index++])) return false;
    }
    return true;
}
