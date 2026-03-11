#include <QTest>
#include <frame_processors/movement_rects_frame_processor/movement_rects_frame_processor.h>

class MovementRectsFrameProcessorTest : public QObject, public MovementRectsFrameProcessor {
    Q_OBJECT

private slots:
    void findLinksForORectsWithMRectsTest();

    void findLinksForMRectsWithORectsTest();

    void findForAddTest();

    void findForAddWithUnharmoniousDataTest();

    void findForRemoveTest();

    void findForRemoveUnharmoniousDataTest();

    void findForMergeTest();

    void findForSplitTest();

    void findForUpdateTest();

    void processChangesAddTest();

    void processChangesUpdateTest();

    void processChangesRemoveTest();

    void processChangesMergeTest();

    void processChangesSplitTest();

private:
    static OpticalFlowRect getORect1();

    static MovementRect getMRect1();

    static OpticalFlowRect getORect2();

    static MovementRect getMRect2();

    static OpticalFlowRect getORect3();

    static MovementRect getMRect3();

    static OpticalFlowRect getORectForMerge1();

    static map<string, MovementRect> getMRectsForMerge1();

    static vector<OpticalFlowRect> getORectsForSplit1();

    static MovementRect getMRectForSplit1();
};

void MovementRectsFrameProcessorTest::findLinksForORectsWithMRectsTest() {
    mRects.clear();
    auto mRect1 = getMRect1();
    auto mRect2 = getMRect2();
    mRects.insert_or_assign(mRect1.getId(), mRect1);
    mRects.insert_or_assign(mRect2.getId(), mRect2);
    auto oRect1 = getORect1();
    auto oRect2 = getORect2();
    vector<OpticalFlowRect> oRects{oRect1, oRect2};
    auto linkedForORectsWithMRects = findLinksForORectsWithMRects(oRects);
    QCOMPARE(linkedForORectsWithMRects.size(), 2);
    for (auto &[oRect,ptrMRect]: linkedForORectsWithMRects) {
        QVERIFY(ptrMRect != nullptr);
        if (oRect == oRect1) {
            QVERIFY(*ptrMRect ==mRect1);
        } else {
            QVERIFY(*ptrMRect == mRect2);
        }
    }
}


void MovementRectsFrameProcessorTest::findLinksForMRectsWithORectsTest() {
    mRects.clear();
    auto mRect1 = getMRect1();
    auto mRect2 = getMRect2();
    mRects.insert_or_assign(mRect1.getId(), mRect1);
    mRects.insert_or_assign(mRect2.getId(), mRect2);
    auto oRect1 = getORect1();
    auto oRect2 = getORect2();
    vector<OpticalFlowRect> oRects{oRect1, oRect2};
    auto linkedForMRectsWithMRects = findLinksForMRectsWithORects(oRects);
    QCOMPARE(linkedForMRectsWithMRects.size(), 2);
    for (auto &[mRect,ptrORect]: linkedForMRectsWithMRects) {
        QVERIFY(ptrORect != nullptr);
        if (mRect == mRect1) {
            QVERIFY(*ptrORect ==oRect1);
        } else {
            QVERIFY(*ptrORect == oRect2);
        }
    }
}

void MovementRectsFrameProcessorTest::findForAddTest() {
    mRects.clear();
    auto mRect1 = getMRect1();
    auto mRect2 = getMRect2();
    mRects.insert_or_assign(mRect1.getId(), mRect1);
    mRects.insert_or_assign(mRect2.getId(), mRect2);
    auto oRect1 = getORect1();
    auto oRect2 = getORect2();
    auto oRect3 = getORect3();
    vector<OpticalFlowRect> oRects{oRect1, oRect2, oRect3};
    auto linkedForORectsWithMRects = findLinksForORectsWithMRects(oRects);
    auto linkedForMRectsWithMRects = findLinksForMRectsWithORects(oRects);
    auto mapORectsForAdd = findForAdd(linkedForORectsWithMRects, linkedForMRectsWithMRects);
    QVERIFY(mapORectsForAdd.size() == 1);
    auto it = mapORectsForAdd.find(oRect3.getId());
    QVERIFY(it != mapORectsForAdd.end());
}

void MovementRectsFrameProcessorTest::findForAddWithUnharmoniousDataTest() {
    mRects.clear();
    auto mRect1 = getMRect1();
    auto mRect2 = getMRect2();
    auto mRect3 = getMRect3();
    mRects.insert_or_assign(mRect1.getId(), mRect1);
    mRects.insert_or_assign(mRect2.getId(), mRect2);
    auto oRect1 = getORect1();
    auto oRect2 = getORect2();
    auto oRect3 = getORect3();
    vector<OpticalFlowRect> oRects{oRect1, oRect2, oRect3};
    auto linkedForORectsWithMRects = findLinksForORectsWithMRects(oRects);
    auto linkedForMRectsWithORects = findLinksForMRectsWithORects(oRects);
    //add unharmonious data
    linkedForMRectsWithORects[mRect3] = make_shared<OpticalFlowRect>(oRect3);
    //
    auto setORectsForAdd = findForAdd(linkedForORectsWithMRects, linkedForMRectsWithORects);
    QVERIFY(setORectsForAdd.size() == 1);
    auto it = setORectsForAdd.find(oRect3.getId());
    QVERIFY(it != setORectsForAdd.end());
    auto itPtrORect = linkedForMRectsWithORects.find(mRect3);
    QVERIFY(itPtrORect != linkedForMRectsWithORects.end() && itPtrORect->second == nullptr);
}

void MovementRectsFrameProcessorTest::findForRemoveTest() {
    mRects.clear();
    auto mRect1 = getMRect1();
    auto mRect2 = getMRect2();
    auto mRect3 = getMRect3();
    mRects.insert_or_assign(mRect1.getId(), mRect1);
    mRects.insert_or_assign(mRect2.getId(), mRect2);
    mRects.insert_or_assign(mRect3.getId(), mRect3);
    auto oRect1 = getORect1();
    auto oRect2 = getORect2();
    vector<OpticalFlowRect> oRects{oRect1, oRect2};
    auto linkedForORectsWithMRects = findLinksForORectsWithMRects(oRects);
    auto linkedForMRectsWithORects = findLinksForMRectsWithORects(oRects);
    map<string, OpticalFlowRect> forAdd;
    auto forDelete = findForRemove(linkedForORectsWithMRects, linkedForMRectsWithORects, forAdd);
    QVERIFY(forDelete.size() == 1);
    auto it = forDelete.find(mRect3.getId());
    QVERIFY(it != forDelete.end() && it->second == mRect3);
}

void MovementRectsFrameProcessorTest::findForRemoveUnharmoniousDataTest() {
    mRects.clear();
    auto mRect1 = getMRect1();
    auto mRect2 = getMRect2();
    auto mRect3 = getMRect3();
    mRects.insert_or_assign(mRect1.getId(), mRect1);
    mRects.insert_or_assign(mRect2.getId(), mRect2);
    mRects.insert_or_assign(mRect3.getId(), mRect3);
    auto oRect1 = getORect1();
    auto oRect2 = getORect2();
    auto oRect3 = getORect3();
    vector<OpticalFlowRect> oRects{oRect1, oRect2};
    auto linkedForORectsWithMRects = findLinksForORectsWithMRects(oRects);
    auto linkedForMRectsWithORects = findLinksForMRectsWithORects(oRects);
    // add unharmonious data
    linkedForORectsWithMRects.insert_or_assign(oRect3, make_shared<MovementRect>(mRect3));
    //
    map<string, OpticalFlowRect> forAdd;
    auto forDelete = findForRemove(linkedForORectsWithMRects, linkedForMRectsWithORects, forAdd);
    QVERIFY(forDelete.size() == 1);
    auto it = forDelete.find(mRect3.getId());
    QVERIFY(it != forDelete.end() && it->second == mRect3);
    QVERIFY(forAdd.size() == 1);
    auto itForAdd = forAdd.find(oRect3.getId());
    QVERIFY(itForAdd != forAdd.end() && itForAdd->second == oRect3);
}

void MovementRectsFrameProcessorTest::findForMergeTest() {
    mRects.clear();
    auto mRectsForMerge = getMRectsForMerge1();
    for (auto &[id,mRectForMerge]: mRectsForMerge) {
        mRectForMerge.setState(MovementRectState::ADDED);
        mRects.insert_or_assign(mRectForMerge.getId(), mRectForMerge);
    }
    auto oRectForMerge = getORectForMerge1();
    vector<OpticalFlowRect> oRects{oRectForMerge};

    auto linkedForORectsWithMRects = findLinksForORectsWithMRects(oRects);
    auto linkedForMRectsWithORects = findLinksForMRectsWithORects(oRects);
    auto forMerge = findForMerge(linkedForORectsWithMRects,
                                 linkedForMRectsWithORects);
    QVERIFY(forMerge.size() == 1);
    auto it = forMerge.find(oRectForMerge);
    QVERIFY(it != forMerge.end());
    auto setMRects = it->second;
    QVERIFY(setMRects.size() == 2);
    for (auto &[id,mRect]: mRectsForMerge) {
        QVERIFY(setMRects.find(mRect.getId()) != setMRects.end());
    }
}

void MovementRectsFrameProcessorTest::findForSplitTest() {
    mRects.clear();
    auto mRectForSplit1 = getMRectForSplit1();
    mRects.insert_or_assign(mRectForSplit1.getId(), mRectForSplit1);
    auto oRects = getORectsForSplit1();
    auto linkedForORectsWithMRects = findLinksForORectsWithMRects(oRects);
    auto linkedForMRectsWithORects = findLinksForMRectsWithORects(oRects);
    auto forSplit = findForSplit(linkedForORectsWithMRects, linkedForMRectsWithORects);
    QVERIFY(forSplit.size() == 1);
    auto it = forSplit.find(mRectForSplit1);
    QVERIFY(it != forSplit.end());
    auto oRectsForSplit = it->second;
    QVERIFY(oRectsForSplit.size() == 2);
    for (auto &oRect: oRects) {
        QVERIFY(oRectsForSplit.find(oRect.getId()) != oRectsForSplit.end());
    }
}

void MovementRectsFrameProcessorTest::findForUpdateTest() {
    mRects.clear();
    auto oRect1 = getORect1();
    auto oRect2 = getORect2();
    auto oRect3 = getORect3();
    auto mRect1 = getMRect1();
    auto mRect2 = getMRect2();
    auto mRect3 = getMRect3();
    mRects.insert_or_assign(mRect1.getId(), mRect1);
    mRects.insert_or_assign(mRect2.getId(), mRect2);
    mRects.insert_or_assign(mRect3.getId(), mRect3);
    vector<OpticalFlowRect> oRects = {oRect1, oRect2, oRect3, oRect2};
    auto linkedForORectsWithMRects = findLinksForORectsWithMRects(oRects);
    auto linkedForMRectsWithORects = findLinksForMRectsWithORects(oRects);
    auto forUpdate = findForUpdate(linkedForORectsWithMRects, linkedForMRectsWithORects);
    QVERIFY(forUpdate.size() == 3);
    QVERIFY(linkedForORectsWithMRects.empty());
    QVERIFY(linkedForMRectsWithORects.empty());
}

void MovementRectsFrameProcessorTest::processChangesAddTest() {
    mRects.clear();
    MovementRectChanges changes;
    auto oRect1 = getORect1();
    auto oRect2 = getORect2();
    changes.forAdd = {{oRect1.getId(), oRect1}, {oRect2.getId(), oRect2}};
    processChanges(changes);
    QVERIFY(mRects.size() == 2);
    for (auto &[id,mRect]: mRects) {
        QVERIFY(mRect.getState() == MovementRectState::ADDING);
    }
}

void MovementRectsFrameProcessorTest::processChangesUpdateTest() {
    mRects.clear();
    MovementRectChanges changes;
    auto oRect1 = getORect1();
    auto oRect2 = getORect2();
    changes.forAdd = {{oRect1.getId(), oRect1}, {oRect2.getId(), oRect2}};
    processChanges(changes);
    QVERIFY(mRects.size() == 2);
    changes.forAdd.clear();

    for (auto &[id, mRect]: mRects) {
        QVERIFY(mRect.getState() == MovementRectState::ADDING);
        if (mRect.getRect() == oRect1.getRect()) {
            changes.forUpdate.insert_or_assign(mRect, oRect1);
        } else {
            changes.forUpdate.insert_or_assign(mRect, oRect2);
        }
    }

    processChanges(changes);
    for (auto &[id,mRect]: mRects) {
        QVERIFY(mRect.getState() == MovementRectState::ADDING);
    }

    processChanges(changes);
    for (auto &[id,mRect]: mRects) {
        QVERIFY(mRect.getState() == MovementRectState::ADDING);
    }

    processChanges(changes);
    for (auto &[id,mRect]: mRects) {
        QVERIFY(mRect.getState() == MovementRectState::ADDED);
    }
}

void MovementRectsFrameProcessorTest::processChangesRemoveTest() {
    mRects.clear();
    MovementRectChanges changes;
    auto mRect1 = getMRect1();
    auto mRect2 = getMRect2();
    mRect1.setState(MovementRectState::ADDED);
    mRect2.setState(MovementRectState::ADDED);
    mRects.insert_or_assign(mRect1.getId(), mRect1);
    mRects.insert_or_assign(mRect2.getId(), mRect2);
    auto oRect1 = getORect1();
    vector<OpticalFlowRect> oRects = {oRect1};
    auto linkedForORectsWithMRects = findLinksForORectsWithMRects(oRects);
    auto linkedForMRectsWithORects = findLinksForMRectsWithORects(oRects);
    map<string, OpticalFlowRect> forAdd;
    auto forRemove = findForRemove(linkedForORectsWithMRects, linkedForMRectsWithORects, forAdd);
    changes.forRemove = forRemove;
    processChanges(changes);
    auto it = mRects.find(mRect2.getId());
    QVERIFY(it != mRects.end());
    QVERIFY(it->second.getState() == MovementRectState::REMOVING);

    processChanges(changes);
    it = mRects.find(mRect2.getId());
    QVERIFY(mRects.size() == 2);
    QVERIFY(it != mRects.end());
    QVERIFY(it->second.getState() == MovementRectState::REMOVING);

    processChanges(changes);
    it = mRects.find(mRect2.getId());
    QVERIFY(mRects.size() == 2);
    QVERIFY(it != mRects.end());
    QVERIFY(it->second.getState() == MovementRectState::REMOVING);

    processChanges(changes);
    it = mRects.find(mRect2.getId());
    QVERIFY(mRects.size() == 1);
    QVERIFY(it == mRects.end());
}

void MovementRectsFrameProcessorTest::processChangesMergeTest() {
    mRects.clear();
    MovementRectChanges changes;
    auto mRectsForMerge = getMRectsForMerge1();
    for (auto &[id,mRectForMerge]: mRectsForMerge) {
        mRectForMerge.setState(MovementRectState::ADDED);
        mRects.insert_or_assign(mRectForMerge.getId(), mRectForMerge);
    }
    auto oRectForMerge = getORectForMerge1();
    vector<OpticalFlowRect> oRects{oRectForMerge};

    auto linkedForORectsWithMRects = findLinksForORectsWithMRects(oRects);
    auto linkedForMRectsWithORects = findLinksForMRectsWithORects(oRects);
    map<OpticalFlowRect, map<string, MovementRect> > forMerge = findForMerge(linkedForORectsWithMRects,
                                                                             linkedForMRectsWithORects);
    changes.forMerge = forMerge;
    processChanges(changes);
    QVERIFY(mRects.size() == 1);
    auto it = mRects.begin();
    QVERIFY(it->second.getRect() == oRectForMerge.getRect());
}

void MovementRectsFrameProcessorTest::processChangesSplitTest() {
    mRects.clear();
    MovementRectChanges changes;
    auto mRectForSplit1 = getMRectForSplit1();
    mRects.insert_or_assign(mRectForSplit1.getId(), mRectForSplit1);
    auto oRects = getORectsForSplit1();
    auto linkedForORectsWithMRects = findLinksForORectsWithMRects(oRects);
    auto linkedForMRectsWithORects = findLinksForMRectsWithORects(oRects);
    auto forSplit = findForSplit(linkedForORectsWithMRects, linkedForMRectsWithORects);
    QVERIFY(forSplit.size() == 1);
    auto it = forSplit.find(mRectForSplit1);
    QVERIFY(it != forSplit.end());
    auto oRectsForSplit = it->second;
    QVERIFY(oRectsForSplit.size() == 2);
    for (auto &oRect: oRects) {
        QVERIFY(oRectsForSplit.find(oRect.getId()) != oRectsForSplit.end());
    }
    changes.forSplit = forSplit;
    processChanges(changes);
    QVERIFY(mRects.size() == 1);
    auto itMRect = mRects.begin();
    QVERIFY(itMRect->second.getState() == MovementRectState::SPLITTING);

    processChanges(changes);
    QVERIFY(mRects.size() == 1);
    itMRect = mRects.begin();
    QVERIFY(itMRect->second.getState() == MovementRectState::SPLITTING);

    processChanges(changes);
    QVERIFY(mRects.size() == 1);
    itMRect = mRects.begin();
    QVERIFY(itMRect->second.getState() == MovementRectState::SPLITTING);

    processChanges(changes);
    QVERIFY(mRects.size() == 2);
    for (auto &[id,mRect]: mRects) {
        auto rect = mRect.getRect();
        bool found = false;
        for (auto &oRect: oRects) {
            if (oRect.getRect() == rect) {
                found = true;
                break;
            }
        }
        QVERIFY(found);
    }
}


OpticalFlowRect MovementRectsFrameProcessorTest::getORect1() {
    auto previousRect = Rect(0, 0, 100, 100);
    auto rect = Rect(5, 5, 100, 100);
    return {rect, previousRect};
}

MovementRect MovementRectsFrameProcessorTest::getMRect1() {
    return MovementRect(Rect(0, 0, 100, 100));
}

OpticalFlowRect MovementRectsFrameProcessorTest::getORect2() {
    auto previousRect = Rect(200, 0, 100, 100);
    auto rect = Rect(205, 5, 100, 100);
    return {rect, previousRect};
}

MovementRect MovementRectsFrameProcessorTest::getMRect2() {
    return MovementRect(Rect(200, 0, 100, 100));
}


OpticalFlowRect MovementRectsFrameProcessorTest::getORect3() {
    auto previousRect = Rect(400, 0, 100, 100);
    auto rect = Rect(405, 5, 100, 100);
    return {rect, previousRect};
}

MovementRect MovementRectsFrameProcessorTest::getMRect3() {
    return MovementRect(Rect(400, 0, 100, 100));
}

OpticalFlowRect MovementRectsFrameProcessorTest::getORectForMerge1() {
    auto previousRect = Rect(600, 0, 200, 100);
    auto rect = Rect(600, 5, 200, 100);
    return {rect, previousRect};
}

map<string, MovementRect> MovementRectsFrameProcessorTest::getMRectsForMerge1() {
    map<string, MovementRect> forMerge;
    auto merge = MovementRect(Rect(590, 0, 100, 100));
    forMerge.insert_or_assign(merge.getId(), merge);
    merge = MovementRect(Rect(710, 0, 100, 100));
    forMerge.insert_or_assign(merge.getId(), merge);
    return forMerge;
}

vector<OpticalFlowRect> MovementRectsFrameProcessorTest::getORectsForSplit1() {
    vector<OpticalFlowRect> forSplit;
    auto previousRect = Rect(600, 200, 100, 100);
    auto rect = Rect(590, 200, 100, 100);
    forSplit.emplace_back(rect, previousRect);
    previousRect = Rect(700, 200, 100, 100);
    rect = Rect(750, 200, 100, 100);
    forSplit.emplace_back(rect, previousRect);
    return forSplit;
}

MovementRect MovementRectsFrameProcessorTest::getMRectForSplit1() {
    return MovementRect(Rect(600, 200, 200, 100));
}


QTEST_MAIN(MovementRectsFrameProcessorTest)
#include "movement_rects_frame_processor_test.moc"
