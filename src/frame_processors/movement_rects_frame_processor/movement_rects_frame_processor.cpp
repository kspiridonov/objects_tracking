#include "movement_rects_frame_processor.h"

#include "core/exception/standart_exception.h"

MovementRectsFrameProcessor::MovementRectsFrameProcessor() : FrameProcessorListener<OpticalFlowRect>(
    "MovedAreasFrameProcessor") {
    opticalFlowFrameProcessor.addListener(this);
}

MovementRectsFrameProcessor::~MovementRectsFrameProcessor() {
    opticalFlowFrameProcessor.removeListener(this);
}

void MovementRectsFrameProcessor::frameProcessed(Mat &frame, const vector<OpticalFlowRect> &oRects) {
    auto changes = getChanges(oRects);
    processChanges(changes);
    vector<MovementRect> rects;
    for (auto &mRect: mRects) {
        auto state = mRect.second.getState();
        if (state != MovementRectState::ADDING) {
            rects.push_back(mRect.second);
        }
    }
    fireFrameProcessed(frame, rects);
}

MovementRectChanges MovementRectsFrameProcessor::getChanges(const vector<OpticalFlowRect> &oRects) {
    MovementRectChanges changes;
    map<OpticalFlowRect, shared_ptr<MovementRect> > linkedORectWithMRect = findLinksForORectsWithMRects(oRects);
    map<MovementRect, shared_ptr<OpticalFlowRect> > linkedMRectWithORect = findLinksForMRectsWithORects(oRects);
    changes.forAdd = findForAdd(linkedORectWithMRect, linkedMRectWithORect);
    changes.forRemove = findForRemove(linkedORectWithMRect, linkedMRectWithORect, changes.forAdd);
    changes.forMerge = findForMerge(linkedORectWithMRect, linkedMRectWithORect);
    changes.forSplit = findForSplit(linkedORectWithMRect, linkedMRectWithORect);
    changes.forUpdate = findForUpdate(linkedORectWithMRect, linkedMRectWithORect);
    return changes;
}

map<OpticalFlowRect, shared_ptr<MovementRect> > MovementRectsFrameProcessor::findLinksForORectsWithMRects(
    const vector<OpticalFlowRect> &oRects) {
    map<OpticalFlowRect, shared_ptr<MovementRect> > linkedORectWithMRect;
    for (auto const &rect: oRects) {
        linkedORectWithMRect[rect] = findLinkedMRect(rect);
    }
    return linkedORectWithMRect;
}

shared_ptr<MovementRect> MovementRectsFrameProcessor::findLinkedMRect(const OpticalFlowRect &rect) {
    shared_ptr<MovementRect> foundedMRect;
    int maxIntersectionSquare = 0;
    Rect otherRect = rect.getPreviousRect();
    for (auto mRect = mRects.begin(); mRect != mRects.end(); ++mRect) {
        int intersectionSquare = mRect->second.getIntersectionSquare(otherRect);
        if (maxIntersectionSquare < intersectionSquare) {
            maxIntersectionSquare = intersectionSquare;
            foundedMRect = make_shared<MovementRect>(mRect->second);
        }
    }
    return foundedMRect;
}


map<MovementRect, shared_ptr<OpticalFlowRect> > MovementRectsFrameProcessor::findLinksForMRectsWithORects(
    const vector<OpticalFlowRect> &oRects) {
    map<MovementRect, shared_ptr<OpticalFlowRect> > linkedMRectWithORect;
    for (auto const &rect: mRects) {
        linkedMRectWithORect[rect.second] = findLinkedORect(rect.second, oRects);
    }
    return linkedMRectWithORect;
}

shared_ptr<OpticalFlowRect> MovementRectsFrameProcessor::findLinkedORect(
    const MovementRect &rect, const vector<OpticalFlowRect> &oRects) {
    shared_ptr<OpticalFlowRect> foundedORect;
    int maxIntersectionSquare = 0;
    Rect otherRect = rect.getRect();
    for (auto oRect = oRects.begin(); oRect != oRects.end(); ++oRect) {
        int intersectionSquare = oRect->getIntersectionSquare(otherRect);
        if (maxIntersectionSquare < intersectionSquare) {
            maxIntersectionSquare = intersectionSquare;
            foundedORect = make_shared<OpticalFlowRect>(*oRect);
        }
    }
    return foundedORect;
}

map<string, OpticalFlowRect> MovementRectsFrameProcessor::findForAdd(
    map<OpticalFlowRect, shared_ptr<MovementRect> > &linkedORectWithMRect,
    map<MovementRect, shared_ptr<OpticalFlowRect> > &linkedMRectWithORect) {
    map<string, OpticalFlowRect> forAdd;
    for (auto it = linkedORectWithMRect.begin(); it != linkedORectWithMRect.end();) {
        auto oRect = it->first;
        if (auto ptrMRect = it->second; ptrMRect == nullptr) {
            if (Logger.isTraceEnabled()) {
                Logger.trace("Found new optical flow rect: " + oRect.toString());
            }
            forAdd.insert_or_assign(oRect.getId(), oRect);
            it = linkedORectWithMRect.erase(it);
            auto ptrMRect1 = isORectPresentIn(oRect, linkedMRectWithORect);
            if (ptrMRect1 != nullptr) {
                linkedMRectWithORect[*ptrMRect1] = nullptr;
                Logger.warn(
                    "For new " + oRect.toString() + " but rect: " + ptrMRect1->toString() +
                    " links to it! Force unlink.");
            }
        } else {
            ++it;
        }
    }
    return forAdd;
}

map<string, MovementRect> MovementRectsFrameProcessor::findForRemove(
    map<OpticalFlowRect, shared_ptr<MovementRect> > &linkedORectWithMRect,
    map<MovementRect, shared_ptr<OpticalFlowRect> > &linkedMRectWithORect,
    map<string, OpticalFlowRect> &forAdd) {
    map<string, MovementRect> forRemove;
    for (auto it = linkedMRectWithORect.begin(); it != linkedMRectWithORect.end();) {
        auto mRect = it->first;
        auto ptrORect = it->second;
        if (ptrORect == nullptr) {
            forRemove.insert_or_assign(mRect.getId(), mRect);
            it = linkedMRectWithORect.erase(it);
            auto ptrORect1 = isMRectPresentIn(mRect, linkedORectWithMRect);
            if (ptrORect1 != nullptr) {
                linkedORectWithMRect.erase(*ptrORect1);
                forAdd.insert_or_assign(ptrORect1->getId(), *ptrORect1);
                Logger.warn(
                    "For delete " + mRect.toString() + " but rect: " + ptrORect1->toString() +
                    " links to it (force unlink)!");
            }
        } else {
            ++it;
        }
    }
    return forRemove;
}


map<OpticalFlowRect, map<string, MovementRect> > MovementRectsFrameProcessor::findForMerge(
    map<OpticalFlowRect, shared_ptr<MovementRect> > &linkedORectWithMRect,
    map<MovementRect, shared_ptr<OpticalFlowRect> > &linkedMRectWithORect) {
    map<OpticalFlowRect, map<string, MovementRect> > forMerge;
    for (auto &[mRect, ptrORect]: linkedMRectWithORect) {
        if (ptrORect != nullptr) {
            forMerge[*ptrORect].insert_or_assign(mRect.getId(), mRect);
        }
    }
    for (auto it = forMerge.begin(); it != forMerge.end();) {
        auto oRect = it->first;
        auto mapMRect = it->second;
        if (mapMRect.size() <= 1) {
            it = forMerge.erase(it);
        } else {
            int numOfAdding = 0;
            for (auto &[id, mRect]: mapMRect) {
                if (mRect.getState() == MovementRectState::ADDING) {
                    Logger.debug("Cancel ADDING STATE for: " + mRect.toString());
                    mRect.setState(MovementRectState::REMOVED);
                    mRects.insert_or_assign(id, mRect);
                    numOfAdding++;
                }
            }
            if (mapMRect.size() - numOfAdding <= 1) {
                it = forMerge.erase(it);
                continue;
            }
            for (auto &[id,mRect]: mapMRect) {
                linkedMRectWithORect.erase(mRect);
            }
            linkedORectWithMRect.erase(oRect);
            ++it;
        }
    }
    return forMerge;
}

map<MovementRect, map<string, OpticalFlowRect> > MovementRectsFrameProcessor::findForSplit(
    map<OpticalFlowRect, shared_ptr<MovementRect> > &linkedORectWithMRect,
    map<MovementRect, shared_ptr<OpticalFlowRect> > &linkedMRectWithORect) {
    map<MovementRect, map<string, OpticalFlowRect> > forSplit;
    for (auto &[oRect, ptrMRect]: linkedORectWithMRect) {
        if (ptrMRect != nullptr) {
            forSplit[*ptrMRect].insert_or_assign(oRect.getId(), oRect);
        }
    }
    for (auto it = forSplit.begin(); it != forSplit.end();) {
        auto mRect = it->first;
        auto mapORect = it->second;
        if (mapORect.size() <= 1) {
            it = forSplit.erase(it);
        } else {
            for (auto &[id,oRect]: mapORect) {
                linkedORectWithMRect.erase(oRect);
            }
            linkedMRectWithORect.erase(mRect);
            ++it;
        }
    }

    return forSplit;
}

map<MovementRect, OpticalFlowRect> MovementRectsFrameProcessor::findForUpdate(
    map<OpticalFlowRect, shared_ptr<MovementRect> > &linkedORectWithMRect,
    map<MovementRect, shared_ptr<OpticalFlowRect> > &linkedMRectWithORect) {
    map<MovementRect, OpticalFlowRect> forUpdate;
    for (auto &[mRect, ptrORect]: linkedMRectWithORect) {
        if (ptrORect != nullptr) {
            forUpdate.insert_or_assign(mRect, *ptrORect);
            if (auto it = linkedORectWithMRect.find(*ptrORect);
                it == linkedORectWithMRect.end() || it->second == nullptr) {
                Logger.warn(
                    "findForUpdate: there is no double link for: " + mRect.toString() + " and " + ptrORect->toString());
            } else if (it->second->getId() != mRect.getId()) {
                Logger.warn(
                    "findForUpdate: there is no double link for: " + mRect.toString() + " and " + ptrORect->toString() +
                    ". Second link to: " + it->second->toString());
            }
        } else {
            Logger.warn("findForUpdate: " + mRect.toString() + " has null link to OpticalFlowRect. Just delete it.");
        }
    }
    for (auto &[mRect, oRect]: forUpdate) {
        linkedORectWithMRect.erase(oRect);
        linkedMRectWithORect.erase(mRect);
    }
    return forUpdate;
}


shared_ptr<MovementRect> MovementRectsFrameProcessor::isORectPresentIn(const OpticalFlowRect &oRect,
                                                                       map<MovementRect, shared_ptr<OpticalFlowRect> > &
                                                                       linkedMRectWithORect) {
    for (auto &[mRect, ptrORect]: linkedMRectWithORect) {
        if (ptrORect != nullptr && ptrORect->getId() == oRect.getId()) {
            return make_shared<MovementRect>(mRect);
        }
    }
    return nullptr;
}


shared_ptr<OpticalFlowRect> MovementRectsFrameProcessor::isMRectPresentIn(const MovementRect &mRect,
                                                                          map<OpticalFlowRect, shared_ptr<
                                                                              MovementRect> > &linkedORectWithMRect) {
    for (auto &[oRect, ptrMRect]: linkedORectWithMRect) {
        if (ptrMRect != nullptr && ptrMRect->getId() == mRect.getId()) {
            return make_shared<OpticalFlowRect>(oRect);
        }
    }
    return nullptr;
}


void MovementRectsFrameProcessor::processChanges(const MovementRectChanges &changes) {
    for (auto &[id,oRectForAdd]: changes.forAdd) {
        auto mRect = MovementRect(oRectForAdd.getRect());
        Logger.debug("Add new movement rect: " + mRect.toString());
        if (auto it = mRects.find(mRect.getId()); it != mRects.end()) {
            auto message = "Something wrong: try to add new but it already exists in set: " + mRect.toString();
            Logger.error(message, StandardException(message));
            mRects.erase(mRect.getId());
        }
        mRects.insert_or_assign(mRect.getId(), mRect);
    }
    for (auto &[id,mRect]: changes.forRemove) {
        if (auto it = mRects.find(mRect.getId()); it != mRects.end()) {
            if (it->second.getState() != MovementRectState::REMOVING) {
                it->second.setState(MovementRectState::REMOVING);
                Logger.debug("Set removing state for rect: " + it->second.toString());
            }
        } else {
            Logger.warn("Can't find rec: " + mRect.toString() + " in local cache!");
        }
    }
    for (auto &[mRect, mapORect]: changes.forSplit) {
        auto it = mRects.find(mRect.getId());
        if (it == mRects.end()) {
            Logger.warn("Can't find rec: " + mRect.toString() + ", in local cache!");
            continue;
        }
        if (it->second.getState() != MovementRectState::SPLITTING && it->second.getState() !=
            MovementRectState::SPLITTED) {
            it->second.setState(MovementRectState::SPLITTING);
            Logger.debug("Set SPLITING state for: " + it->second.toString());
        }
        it->second.incDebounce();
        if (it->second.getState() == MovementRectState::SPLITTED) {
            string prefixLog;
            for (auto &[id,oRect]: mapORect) {
                auto mRectsSplitted = MovementRect(oRect.getRect());
                mRectsSplitted.setState(MovementRectState::ADDED);
                mRectsSplitted.addParentId(it->second.getId());
                mRects.insert_or_assign(mRectsSplitted.getId(), mRectsSplitted);
                prefixLog += " rect: " + mRectsSplitted.toString() + ",";
            }
            Logger.debug("Split movement rect: " + it->second.toString() + " to " + prefixLog);
            mRects.erase(it->second.getId());
        }
    }
    for (auto &[id,mRect]: mRects) {
        if (mRect.getState() == MovementRectState::SPLITTING) {
            auto it1 = changes.forSplit.find(mRect);
            if (it1 == changes.forSplit.end()) {
                mRect.setState(MovementRectState::ADDED);
                Logger.debug("Reverting SPLITTING for rect: " + mRect.toString());
            }
        }
    }

    for (auto &[oRect, mapMRect]: changes.forMerge) {
        auto merged = MovementRect(oRect.getRect());
        merged.setState(MovementRectState::ADDED);
        string prefixLog;
        for (auto &[id,mRect]: mapMRect) {
            prefixLog += " " + mRect.toString() + ",";
            mRects.erase(mRect.getId());
            merged.addParentId(mRect.getId());
        }
        mRects.insert_or_assign(merged.getId(), merged);
        Logger.debug("Merge movement rects: " + prefixLog + " into rect: " + merged.toString());
    }

    for (auto &[mRect, oRect]: changes.forUpdate) {
        auto it = mRects.find(mRect.getId());
        if (it != mRects.end()) {
            it->second.setRect(oRect.getRect());
        } else {
            Logger.warn("Can't find rect: " + mRect.toString() + " in local cache");
        }
    }
    for (auto it = mRects.begin(); it != mRects.end();) {
        if (it->second.getState() == MovementRectState::SPLITTING) {
            ++it;
            continue;
        }
        it->second.incDebounce();
        if (it->second.getState() == MovementRectState::REMOVED) {
            Logger.trace("Delete rect(with REMOVED state) from local cache: " + it->second.toString());
            it = mRects.erase(it);
        } else {
            ++it;
        }
    }
}


void MovementRectsFrameProcessor::processFrame(Mat &currentFrame) {
    opticalFlowFrameProcessor.processFrame(currentFrame);
}
