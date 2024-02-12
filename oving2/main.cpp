#include <iostream>
#include <vector>
#include <thread>
#include <queue>


class Workers {
public:
    Workers(size_t threadCount) : stopAll(false), started(false) {
        // Reserver plass for trådene for å unngå reallokeringer senere.
        workers.reserve(threadCount);
    }

    ~Workers() {
        stop(); // Når objektet ødelegges, stopp alle trådene.
    }

    void start() {
        // Lås for å unngå race condition under oppstart.
        std::unique_lock<std::mutex> lock(tasksMutex);
        if (!started) { // Forhindre at tråder startes flere ganger.
            for (size_t i = 0; i < workers.capacity(); ++i) {
                workers.emplace_back(&Workers::workerFunction, this); // Legg til tråd i vektoren.
            }
            started = true; // Sett flagget til true for å indikere at trådene er startet.
        }
    }


    void post(const std::function<void()>& task) {
        {
            std::unique_lock<std::mutex> lock(tasksMutex);
            tasks.push(task); // Legg oppgaven til i køen.
        }
        condVar.notify_one(); // Vekk en tråd som venter på en oppgave.
    }

    void post_timeout(const std::function<void()>& task, unsigned int delay_ms) {
        std::thread([this, task, delay_ms]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms)); // Vent for en viss tid.
            post(task); // Legg oppgaven til i køen etter forsinkelsen.
        }).detach(); // Detach tråden fordi den kjører som en egen enhet og ikke trenger å bli joinet.
    }

    void stop() {
        {
            std::unique_lock<std::mutex> lock(tasksMutex);
            stopAll = true; // Sett flagget til true for å signalisere at trådene skal avsluttes.
        }
        condVar.notify_all(); // Vekk alle trådene som venter på en oppgave.
        for (auto& worker : workers) {
            if (worker.joinable()) {
                worker.join(); // Vent på at hver tråd blir ferdig.
            }
        }
        workers.clear(); // Fjern trådreferansene etter at de er blitt joinet.
        started = false; // Tilbakestill flagget for oppstart.
    }

private:
    std::vector<std::thread> workers; // Lagre trådene i en vektor.
    std::queue<std::function<void()>> tasks; // Kø for oppgaver som skal utføres.
    std::mutex tasksMutex; // Mutex for å beskytte tilgang til oppgavekøen.
    std::condition_variable condVar; // Condition variable for å synkronisere trådene.
    bool stopAll; // Flagget som indikerer om trådene skal avsluttes.
    bool started; // Flagget som indikerer om trådene allerede er startet.

    void workerFunction() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(tasksMutex);
                condVar.wait(lock, [this] {
                    return stopAll || !tasks.empty(); // Vent på oppgaver eller stoppsignal.
                });
                if (stopAll && tasks.empty()) break; // Avslutt hvis stoppsignal er mottatt og ingen oppgaver er igjen.
                task = std::move(tasks.front()); // Hent oppgaven fra køen.
                tasks.pop(); // Fjern oppgaven fra køen.
            }
            task(); // Utfør oppgaven.
        }
    }
};

int main() {
    Workers worker_threads(4); // Opprett 4 worker-tråder.
    Workers event_loop(1);     // Opprett en enkelt tråd for event loop.

    worker_threads.start();    // Start worker-trådene.
    event_loop.start();        // Start event loop-tråden.

    // Legg til oppgaver i køen for worker-trådene og event loop.
    worker_threads.post([]() {
        std::cout << "Task A" << std::endl;
    });
    worker_threads.post([]() {
        std::cout << "Task B" << std::endl;
    });
    event_loop.post([]() {
        std::cout << "Task C" << std::endl;
    });
    event_loop.post_timeout([]() {
        std::cout << "Task D" << std::endl;
    }, 5000); // Kjør Task D etter 5000 millisekunder.

    // La programmet kjøre i 10 sekunder.
    std::this_thread::sleep_for(std::chrono::seconds(10));

    // Stopp worker-trådene og event loop.
    worker_threads.stop();
    event_loop.stop();

    return 0;
}






/*
#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <chrono>

class Workers {
public:
    Workers(size_t threadCount) : stopAll(false), started(false) {
        // Reserver plass for trådene for å unngå reallokeringer senere.
        workers.reserve(threadCount);
    }

    ~Workers() {
        stop(); // Når objektet ødelegges, stopp alle trådene.
    }

    void start() {
        // Lås for å unngå race condition under oppstart.
        std::unique_lock<std::mutex> lock(tasksMutex);
        if (!started) { // Forhindre at tråder startes flere ganger.
            for (size_t i = 0; i < workers.capacity(); ++i) {
                workers.emplace_back(&Workers::workerFunction, this); // Legg til tråd i vektoren.
            }
            started = true; // Sett flagget til true for å indikere at trådene er startet.
        }
    }

    void post(const std::function<void()>& task, const std::string& taskName) {
        {
            std::unique_lock<std::mutex> lock(tasksMutex);
            tasks.push(std::make_pair(task, taskName)); // Legg oppgaven og navnet til oppgaven i køen.
        }
        condVar.notify_one(); // Vekk en tråd som venter på en oppgave.
    }

    void post_timeout(const std::function<void()>& task, unsigned int delay_ms, const std::string& taskName) {
        std::thread([this, task, delay_ms, taskName]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms)); // Vent for en viss tid.
            post(task, taskName); // Legg oppgaven til i køen etter forsinkelsen.
        }).detach(); // Detach tråden fordi den kjører som en egen enhet og ikke trenger å bli joinet.
    }

    void stop() {
        {
            std::unique_lock<std::mutex> lock(tasksMutex);
            stopAll = true; // Sett flagget til true for å signalisere at trådene skal avsluttes.
        }
        condVar.notify_all(); // Vekk alle trådene som venter på en oppgave.
        for (auto& worker : workers) {
            if (worker.joinable()) {
                worker.join(); // Vent på at hver tråd blir ferdig.
            }
        }
        workers.clear(); // Fjern trådreferansene etter at de er blitt joinet.
        started = false; // Tilbakestill flagget for oppstart.
    }

private:
    std::vector<std::thread> workers; // Lagre trådene i en vektor.
    std::queue<std::pair<std::function<void()>, std::string>> tasks; // Kø for oppgaver som skal utføres sammen med navnene deres.
    std::mutex tasksMutex; // Mutex for å beskytte tilgang til oppgavekøen.
    std::condition_variable condVar; // Condition variable for å synkronisere trådene.
    bool stopAll; // Flagget som indikerer om trådene skal avsluttes.
    bool started; // Flagget som indikerer om trådene allerede er startet.

    void workerFunction() {
        while (true) {
            std::pair<std::function<void()>, std::string> taskPair;
            {
                std::unique_lock<std::mutex> lock(tasksMutex);
                condVar.wait(lock, [this] {
                    return stopAll || !tasks.empty(); // Vent på oppgaver eller stoppsignal.
                });
                if (stopAll && tasks.empty()) break; // Avslutt hvis stoppsignal er mottatt og ingen oppgaver er igjen.
                taskPair = std::move(tasks.front()); // Hent oppgaven og navnet fra køen.
                tasks.pop(); // Fjern oppgaven fra køen.
            }
            std::cout << "Executing task " << taskPair.second << std::endl; // Systemutskrift før oppgaven utføres.
            taskPair.first(); // Utfør oppgaven.
            std::cout << "Finished task " << taskPair.second << std::endl; // Systemutskrift etter at oppgaven er utført.
        }
    }
};

int main() {
    Workers worker_threads(4); // Opprett 4 worker-tråder.
    Workers event_loop(1);     // Opprett en enkelt tråd for event loop.

    worker_threads.start();    // Start worker-trådene.
    event_loop.start();        // Start event loop-tråden.

    // Legg til oppgaver i køen for worker-trådene og event loop.
    worker_threads.post([]() {
        std::cout << "Starting task A" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2)); // Simulerer en lang kjøretid.
    }, "A");
    worker_threads.post([]() {
        std::cout << "Starting task B" << std::endl;
    }, "B");
    event_loop.post([]() {
        std::cout << "Starting task C" << std::endl;
    }, "C");
    event_loop.post_timeout([]() {
        std::cout << "Starting task D" << std::endl;
    }, 5000, "D"); // Kjør Task D etter 5000 millisekunder.

    // La programmet kjøre i 10 sekunder.
    std::this_thread::sleep_for(std::chrono::seconds(10));

    // Stopp worker-trådene og event loop.
    worker_threads.stop();
    event_loop.stop();

    return 0;
}
*/