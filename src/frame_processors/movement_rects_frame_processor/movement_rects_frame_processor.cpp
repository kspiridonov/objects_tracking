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
        auto state = mRect.getState();
        if (state != MovementRectState::ADDING) {
            rects.push_back(mRect);
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
        int intersectionSquare = mRect->getIntersectionSquare(otherRect);
        if (maxIntersectionSquare < intersectionSquare) {
            maxIntersectionSquare = intersectionSquare;
            foundedMRect = make_shared<MovementRect>(*mRect);
        }
    }
    return foundedMRect;
}


map<MovementRect, shared_ptr<OpticalFlowRect> > MovementRectsFrameProcessor::findLinksForMRectsWithORects(
    const vector<OpticalFlowRect> &oRects) {
    map<MovementRect, shared_ptr<OpticalFlowRect> > linkedMRectWithORect;
    for (auto const &rect: mRects) {
        linkedMRectWithORect[rect] = findLinkedORect(rect, oRects);
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

set<OpticalFlowRect> MovementRectsFrameProcessor::findForAdd(
    map<OpticalFlowRect, shared_ptr<MovementRect> > &linkedORectWithMRect,
    map<MovementRect, shared_ptr<OpticalFlowRect> > &linkedMRectWithORect) {
    set<OpticalFlowRect> forAdd;
    for (auto &[oRect, ptrMRect]: linkedORectWithMRect) {
        if (ptrMRect == nullptr) {
            if (Logger.isTraceEnabled()) {
                Logger.trace("Found new optical flow rect: " + oRect.toString());
            }
            forAdd.insert(oRect);
            auto ptrMRect1 = isORectPresentIn(oRect, linkedMRectWithORect);
            if (ptrMRect1 != nullptr) {
                linkedMRectWithORect[*ptrMRect1] = nullptr;
                Logger.warn(
                    "For new " + oRect.toString() + " but rect: " + ptrMRect1->toString() +
                    " links to it! Force unlink.");
            }
        }
    }
    for (auto &oRect: forAdd) {
        linkedORectWithMRect.erase(oRect);
    }
    return forAdd;
}

set<MovementRect> MovementRectsFrameProcessor::findForRemove(
    map<OpticalFlowRect, shared_ptr<MovementRect> > &linkedORectWithMRect,
    map<MovementRect, shared_ptr<OpticalFlowRect> > &linkedMRectWithORect,
    set<OpticalFlowRect> &forAdd) {
    set<MovementRect> forRemove;
    for (auto &[mRect, ptrORect]: linkedMRectWithORect) {
        if (ptrORect == nullptr) {
            forRemove.insert(mRect);
            auto ptrORect1 = isMRectPresentIn(mRect, linkedORectWithMRect);
            if (ptrORect1 != nullptr) {
                linkedORectWithMRect.erase(*ptrORect1);
                forAdd.insert(*ptrORect1);
                Logger.warn(
                    "For delete " + mRect.toString() + " but rect: " + ptrORect1->toString() +
                    " links to it (force unlink)!");
            }
        }
    }
    for (auto &mRect: forRemove) {
        linkedMRectWithORect.erase(mRect);
    }
    return forRemove;
}


map<OpticalFlowRect, set<MovementRect> > MovementRectsFrameProcessor::findForMerge(
    map<OpticalFlowRect, shared_ptr<MovementRect> > &linkedORectWithMRect,
    map<MovementRect, shared_ptr<OpticalFlowRect> > &linkedMRectWithORect) {
    map<OpticalFlowRect, set<MovementRect> > forMerge;
    for (auto &[mRect, ptrORect]: linkedMRectWithORect) {
        if (ptrORect != nullptr) {
            forMerge[*ptrORect].insert(mRect);
        }
    }
    set<OpticalFlowRect> excludedFromMerge;
    for (auto &[oRect, setMRect]: forMerge) {
        if (setMRect.size() <= 1) {
            excludedFromMerge.insert(oRect);
        } else {
            int numOfAdding = 0;
            for (auto mRect: setMRect) {
                if (mRect.getState() == MovementRectState::ADDING) {
                    Logger.debug("Cancel ADDING STATE for: " + mRect.toString());
                    mRect.setState(MovementRectState::REMOVED);
                    mRects.erase(mRect);
                    mRects.insert(mRect);
                    numOfAdding++;
                }
            }
            if (setMRect.size() - numOfAdding <= 1) {
                excludedFromMerge.insert(oRect);
                continue;
            }
            for (auto &mRect: setMRect) {
                linkedMRectWithORect.erase(mRect);
            }
            linkedORectWithMRect.erase(oRect);
        }
    }
    for (auto &oRect: excludedFromMerge) {
        forMerge.erase(oRect);
    }
    return forMerge;
}

map<MovementRect, set<OpticalFlowRect> > MovementRectsFrameProcessor::findForSplit(
    map<OpticalFlowRect, shared_ptr<MovementRect> > &linkedORectWithMRect,
    map<MovementRect, shared_ptr<OpticalFlowRect> > &linkedMRectWithORect) {
    map<MovementRect, set<OpticalFlowRect> > forSplit;
    for (auto &[oRect, ptrMRect]: linkedORectWithMRect) {
        if (ptrMRect != nullptr) {
            forSplit[*ptrMRect].insert(oRect);
        }
    }
    set<MovementRect> excludedFromSplit;
    for (auto &[mRect, setORect]: forSplit) {
        if (setORect.size() <= 1) {
            excludedFromSplit.insert(mRect);
        } else {
            for (auto &oRect: setORect) {
                linkedORectWithMRect.erase(oRect);
            }
            linkedMRectWithORect.erase(mRect);
        }
    }
    for (auto &mRect: excludedFromSplit) {
        forSplit.erase(mRect);
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
    for (auto &oRectForAdd: changes.forAdd) {
        auto mRect = MovementRect(oRectForAdd.getRect());
        Logger.debug("Add new movement rect: " + mRect.toString());
        if (auto it = mRects.find(mRect); it != mRects.end()) {
            auto message = "Something wrong: try to add new but it already exists in set: " + mRect.toString();
            Logger.error(message, StandardException(message));
            mRects.erase(mRect);
        }
        mRects.insert(mRect);
    }
    for (MovementRect mRect: changes.forRemove) {
        auto it = mRects.find(mRect);
        if (it != mRects.end()) {
            if (auto mRectForRemove = *it; mRectForRemove.getState() != MovementRectState::REMOVING) {
                mRectForRemove.setState(MovementRectState::REMOVING);
                mRects.erase(mRectForRemove);
                mRects.insert(mRectForRemove);
                Logger.debug("Set removing state for rect: " + mRectForRemove.toString());
            }
        } else {
            Logger.warn("Can't find rec: " + mRect.toString() + " in local cache!");
        }
    }
    for (auto &[mRect, setORect]: changes.forSplit) {
        auto it = mRects.find(mRect);
        if (it == mRects.end()) {
            Logger.warn("Can't find rec: " + mRect.toString() + ", in local cache!");
            continue;
        }
        auto updatedMRect = *it;
        if (it->getState() != MovementRectState::SPLITTING && it->getState() != MovementRectState::SPLITTED) {
            updatedMRect.setState(MovementRectState::SPLITTING);
            Logger.debug("Set SPLITING state for: " + updatedMRect.toString());
        }
        updatedMRect.incDebounce();
        mRects.erase(updatedMRect);
        if (updatedMRect.getState() == MovementRectState::SPLITTED) {
            string prefixLog;
            for (auto &oRect: setORect) {
                auto mRectsSplitted = MovementRect(oRect.getRect());
                mRectsSplitted.setState(MovementRectState::ADDED);
                mRectsSplitted.addParentId(updatedMRect.getId());
                mRects.insert(mRectsSplitted);
                prefixLog += " rect: " + mRectsSplitted.toString() + ",";
            }
            Logger.debug("Split movement rect: " + updatedMRect.toString() + " to " + prefixLog);
        } else {
            mRects.insert(updatedMRect);
        }
    }
    set<MovementRect> mRectsForRevertSplitting;
    for (auto &mRect: mRects) {
        if (mRect.getState() == MovementRectState::SPLITTING) {
            auto it = changes.forSplit.find(mRect);
            if (it == changes.forSplit.end()) {
                mRectsForRevertSplitting.insert(mRect);
            }
        }
    }
    for (auto &mRect: mRectsForRevertSplitting) {
        auto update = mRect;
        update.setState(MovementRectState::ADDED);
        Logger.debug("Reverting SPLITTING for rect: " + update.toString());
        mRects.erase(update);
        mRects.insert(update);
    }
    for (auto &[oRect, setMRect]: changes.forMerge) {
        auto merged = MovementRect(oRect.getRect());
        merged.setState(MovementRectState::ADDED);
        string prefixLog;
        for (auto &mRect: setMRect) {
            prefixLog += " " + mRect.toString() + ",";
            mRects.erase(mRect);
            merged.addParentId(mRect.getId());
        }
        if (auto it = mRects.find(merged); it != mRects.end()) {
            auto message = "Something wrong: try to add merged but it already exists in set: " + merged.toString();
            Logger.error(message, StandardException(message));
            mRects.erase(merged);
        }
        mRects.insert(merged);
        Logger.debug("Merge movement rects: " + prefixLog + " into rect: " + merged.toString());
    }
    for (auto &[mRect, oRect]: changes.forUpdate) {
        auto it = mRects.find(mRect);
        if (it != mRects.end()) {
            MovementRect updatedMRect = *it;
            updatedMRect.setRect(oRect.getRect());
            mRects.erase(mRect);
            mRects.insert(updatedMRect);
        } else {
            Logger.warn("Can't find rect: " + mRect.toString() + " in local cache");
        }
    }
    set<MovementRect> mRectsWithRemovedStatus;
    set<MovementRect> forUpdate;
    for (auto &mRect: mRects) {
        if (mRect.getState() == MovementRectState::SPLITTING) { continue; }
        MovementRect updatedMRect = mRect;
        updatedMRect.incDebounce();
        forUpdate.insert(updatedMRect);
        if (updatedMRect.getState() == MovementRectState::REMOVED) {
            mRectsWithRemovedStatus.insert(updatedMRect);
        }
    }
    for (auto &mRect: forUpdate) {
        mRects.erase(mRect);
        mRects.insert(mRect);
    }
    for (auto &mRect: mRectsWithRemovedStatus) {
        if (Logger.isTraceEnabled()) {
            Logger.trace("Delete rect(with DELETED state) from locak cache: " + mRect.toString());
        }
        mRects.erase(mRect);
    }
}


void MovementRectsFrameProcessor::processFrame(Mat &currentFrame) {
    opticalFlowFrameProcessor.processFrame(currentFrame);
}
