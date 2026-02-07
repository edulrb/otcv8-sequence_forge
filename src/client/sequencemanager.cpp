#include "sequencemanager.h"
#include <framework/otml/otml.h>
#include "declarations.h"
#include <framework/core/resourcemanager.h>

SequenceManager g_sequences;

void SequenceManager::init()
{
}

void SequenceManager::terminate()
{
    m_sequences.clear();
}

SequencePtr SequenceManager::getSequence(int id)
{
    auto it = m_sequences.find(id);
    if (it != m_sequences.end()) {
        return it->second;
    }
    return nullptr;
}

void SequenceManager::load(const std::string& fileName)
{
    try {
        std::string fullPath = g_resources.guessFilePath(fileName, "otml");
        if (!g_resources.fileExists(fullPath)) {
            g_logger.error(stdext::format("Sequence file not found: %s", fullPath));
            return;
        }

        OTMLDocumentPtr doc = OTMLDocument::parse(fullPath);
        m_sequences.clear();

        for (const OTMLNodePtr& sequencesNode : doc->children()) {
            if (sequencesNode->tag() != "sequences") continue;

            for (const OTMLNodePtr& idNode : sequencesNode->children()) {
                int sequenceId = stdext::safe_cast<int>(idNode->tag());
                SequencePtr seq(new Sequence());
                
                std::map<int, SequenceFrame> keyframes; // Mapa para guardar os keyframes ordenados

                for (const OTMLNodePtr& propNode : idNode->children()) {
                    if (propNode->tag() == "name") {
                        seq->setName(propNode->value());
                    }
                    else if (propNode->tag() == "frame-rate") {
                        int frameRate = stdext::safe_cast<int>(propNode->value());
                        seq->setFrameRate(frameRate > 0 ? frameRate : 1);
                    }
                    else if (propNode->tag() == "frames") {
                        for (const OTMLNodePtr& frameNode : propNode->children()) {
                            std::string seqString = frameNode->value();
                            std::vector<std::string> values = stdext::split<std::string>(seqString, " ");

                            if (values.size() >= 4) {
                                SequenceFrame keyframe;
                                keyframe.frameNumber = stdext::safe_cast<int>(values[0]);

                                if (values[1] == "a" || values[1] == "A") {
                                    keyframe.direction = Otc::InvalidDirection;
                                } else {
                                    int dirNum = stdext::safe_cast<int>(values[1]);
                                    switch (dirNum) {
                                        case 1: keyframe.direction = Otc::South; break;
                                        case 2: keyframe.direction = Otc::East; break;
                                        case 3: keyframe.direction = Otc::North; break;
                                        case 4: keyframe.direction = Otc::West; break;
                                        default: keyframe.direction = Otc::InvalidDirection; break;
                                    }
                                }
                                keyframe.offset = Point(stdext::safe_cast<int>(values[2]), stdext::safe_cast<int>(values[3]));
                                keyframes[keyframe.frameNumber] = keyframe;
                            }
                        }
                    }
                }

                // [[ LÓGICA DE INTERPOLAÇÃO ]]
                if (keyframes.empty()) continue;

                auto it = keyframes.begin();
                SequenceFrame lastFrame = it->second;
                lastFrame.frameNumber = 1; // Garante que a primeira animação começa no frame 1
                if (keyframes.find(1) == keyframes.end()) {
                    lastFrame.offset = Point(0, 0); // Se o frame 1 não for definido, começa do 0,0
                }

                for (; it != keyframes.end(); ++it) {
                    SequenceFrame currentFrame = it->second;
                    int frameDiff = currentFrame.frameNumber - lastFrame.frameNumber;

                    // Interpola os frames entre o último keyframe e o atual
                    for (int i = 1; i <= frameDiff; ++i) {
                        float progress = (float)i / frameDiff;
                        SequenceFrame newFrame;
                        newFrame.offset.x = std::round(lastFrame.offset.x + (currentFrame.offset.x - lastFrame.offset.x) * progress);
                        newFrame.offset.y = std::round(lastFrame.offset.y + (currentFrame.offset.y - lastFrame.offset.y) * progress);
                        newFrame.direction = currentFrame.direction; // O frame interpolado assume a direção do seu keyframe final
                        seq->addFrame(newFrame);
                    }
                    lastFrame = currentFrame;
                }
                m_sequences[sequenceId] = seq;
            }
        }
    }
    catch (std::exception& e) {
        g_logger.error(stdext::format("Failed to load sequences from %s: %s", fileName, e.what()));
    }
}