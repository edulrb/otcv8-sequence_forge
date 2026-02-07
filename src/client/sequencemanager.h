#ifndef SEQUENCEMANAGER_H
#define SEQUENCEMANAGER_H

#include "declarations.h"
#include <framework/otml/declarations.h>

// Gerenciador Singleton para carregar e acessar todas as sequências
class SequenceManager
{
public:
    void init();
    void terminate();

    void load(const std::string& fileName);
    SequencePtr getSequence(int id);

private:
    std::map<int, SequencePtr> m_sequences;
};

extern SequenceManager g_sequences;

#endif