#include <iostream>
#include <time.h>

using namespace std;

#define NAME_SIZE 32
#define INVENTORY_MAX 20
#define LEVEL_MAX 10
#define STORE_WEAPON_MAX 3
#define STORE_ARMOR_MAX 3
#define ITEM_DESC_LENGHT 512

enum MAIN_MENU
{
    MM_NONE,
    MM_MAP,
    MM_STORE,
    MM_INVENTORY,
    MM_EXIT
};

enum JOB
{
    JOB_NONE,
    JOB_KNIGHT,
    JOB_ARCHER,
    JOB_WIZARD,
    JOB_END
};

enum ITEM_TYPE
{
    IT_NONE,
    IT_WEAPON,
    IT_ARMOR,
    IT_BACK
};

enum EQUIP
{
    EQ_WEAPON,
    EQ_ARMOR,
    EQ_MAX
};

enum MAP_TYPE
{
    MT_NONE,
    MT_EASY,
    MT_NORMAL,
    MT_HARD,
    MT_BACK
};

enum BATTLE
{
    BATTLE_NONE,
    BATTLE_ATTACK,
    BATTLE_BACK
};

enum STORE_MENU
{
    SM_NONE,
    SM_WEAPON,
    SM_ARMOR,
    SM_BACK
};

struct _tagItem
{
    char strName[NAME_SIZE];
    char strTypeName[NAME_SIZE];
    char strDesc[ITEM_DESC_LENGHT];
    ITEM_TYPE eType;
    int iMin;
    int iMax;
    int iPrice;
    int iSell;
};

struct _tagInventory
{
    _tagItem tItem[INVENTORY_MAX];
    int iItemCount;
    int iGold;
};

struct _tagPlayer
{
    char strName[NAME_SIZE];
    char strJobName[NAME_SIZE];
    JOB eJob;
    int iAttackMin;
    int iAttackMax;
    int iArmorMin;
    int iArmorMax;
    int iHP;
    int iHPMax;
    int iMP;
    int iMPMax;
    int iExp;
    int iLevel;
    _tagItem tEquip[EQ_MAX];
    bool bEquip[EQ_MAX];
    _tagInventory tInventory;
};

struct _tagMonster
{
    char strName[NAME_SIZE];
    int iAttackMin;
    int iAttackMax;
    int iArmorMin;
    int iArmorMax;
    int iHP;
    int iHPMax;
    int iMP;
    int iMPMax;
    int iLevel;
    int iExp;
    int iGoldMin;
    int iGoldMax;
};

struct _tagLevelUpStatus
{
    int iAttackMin;
    int iAttackMax;
    int iArmorMin;
    int iArmorMax;
    int iHPMin;
    int iHPMax;
    int iMPMin;
    int iMPMax;
};

// 레벨업에 필요한 경험치 목록을 만든다.
const int g_iLevelUpExp[LEVEL_MAX] = {4000, 10000, 20000, 35000, 50000, 70000, 100000,
                                      150000, 200000, 400000};

_tagLevelUpStatus g_tLvUpTable[JOB_END - 1] = {};

// 숫자를 입력 받는다. 입력 오류까지 처리해주고 INT_MAX를 리턴하면
// 입력 오류
int InputInt()
{
    int iInput;
    cin >> iInput;

    if (cin.fail())
    {
        cin.clear();
        cin.ignore(1024, '\n');
        return INT_MAX;
    }

    return iInput;
}

// 직업 선택
int SelectJob()
{
    int iJob = JOB_NONE;

    while (iJob == JOB_NONE)
    {
        system("cls");
        cout << "1. 기사" << endl;
        cout << "2. 궁수" << endl;
        cout << "3. 마법사" << endl;
        cout << "직업을 선택하세요. : ";
        iJob = InputInt();

        if (iJob <= JOB_NONE || iJob >= JOB_END)
            iJob = JOB_NONE;
    }
    return iJob;
}

// 플레이어 설정하기
void SetPlayer(_tagPlayer *pPlayer)
{
    // 플레이어 이름을 입력 받는다.
    cout << "이름 : ";
    cin.getline(pPlayer->strName, NAME_SIZE - 1);

    pPlayer->eJob = (JOB)SelectJob();
    pPlayer->iLevel = 1;
    pPlayer->iExp = 0;
    pPlayer->tInventory.iGold = 10000;

    switch (pPlayer->eJob)
    {
    case JOB_KNIGHT:
        strcpy_s(pPlayer->strJobName, "기사");
        pPlayer->iAttackMin = 5;
        pPlayer->iAttackMax = 10;
        pPlayer->iArmorMin = 15;
        pPlayer->iArmorMax = 20;
        pPlayer->iHP = 500;
        pPlayer->iHPMax = 500;
        pPlayer->iMP = 100;
        pPlayer->iMPMax = 100;
        break;
    case JOB_ARCHER:
        strcpy_s(pPlayer->strJobName, "궁수");
        pPlayer->iAttackMin = 10;
        pPlayer->iAttackMax = 15;
        pPlayer->iArmorMin = 10;
        pPlayer->iArmorMax = 15;
        pPlayer->iHP = 400;
        pPlayer->iHPMax = 400;
        pPlayer->iMP = 200;
        pPlayer->iMPMax = 200;
        break;
    case JOB_WIZARD:
        strcpy_s(pPlayer->strJobName, "마법사");
        pPlayer->iAttackMin = 20;
        pPlayer->iAttackMax = 40;
        pPlayer->iArmorMin = 5;
        pPlayer->iArmorMax = 10;
        pPlayer->iHP = 300;
        pPlayer->iHPMax = 300;
        pPlayer->iMP = 300;
        pPlayer->iMPMax = 300;
        break;
    }
}

// 로비 메뉴 선택
int OutPutMenu()
{
    int iMenu;

    system("cls");
    cout << "******************************** 로비 ********************************" << endl;
    cout << "1. 맵" << endl;
    cout << "2. 상점" << endl;
    cout << "3. 가방" << endl;
    cout << "4. 종료" << endl;
    cout << "메뉴를 선택하세요. : ";
    iMenu = InputInt();

    if (iMenu == INT_MAX || iMenu <= MM_NONE || iMenu > MM_EXIT)
        return MM_NONE;

    return iMenu;
}

// 몬스터 생성
_tagMonster CreatMonster(const char *pName, int iAttackMin, int iAttackMax, int iArmorMin,
                         int iArmorMax, int iHP, int iMP, int iLevel, int iExp, int iGoldMin, int iGoldMax)
{
    _tagMonster tMonster = {};
    // 구조체 배열에 값을 넣기 위해서 strcpy_s() 함수를 사용함
    strcpy_s(tMonster.strName, pName);
    tMonster.iAttackMin = iAttackMin;
    tMonster.iAttackMax = iAttackMax;
    tMonster.iArmorMin = iArmorMin;
    tMonster.iArmorMax = iArmorMax;
    tMonster.iHP = iHP;
    tMonster.iHPMax = iHP;
    tMonster.iMP = iMP;
    tMonster.iMPMax = iMP;
    tMonster.iLevel = iLevel;
    tMonster.iExp = iExp;
    tMonster.iGoldMin = iGoldMin;
    tMonster.iGoldMax = iGoldMax;

    return tMonster;
}

// 몬스터에 값을 넣어주는 함수
void SetMonster(_tagMonster *pMonsterArr)
{
    pMonsterArr[0] = CreatMonster("고블린", 20, 30, 2, 5, 100, 10, 1, 1000, 500, 1500);
    pMonsterArr[1] = CreatMonster("트롤", 80, 130, 60, 90, 2000, 100, 5, 7000, 6000, 8000);
    pMonsterArr[2] = CreatMonster("드래곤", 250, 500, 200, 400, 30000, 20000, 10, 30000,
                                  20000, 50000);
}

// 맵 리스트를 보여주고 선택
int OutPutMapMenu()
{
    int iMenu;

    system("cls");
    cout << "******************************** 맵 **************************" << endl;
    cout << "1. 쉬움" << endl;
    cout << "2. 보통" << endl;
    cout << "3. 어려움" << endl;
    cout << "4. 뒤로가기" << endl;
    cout << "맵을 선택하세요. :";
    iMenu = InputInt();

    if (iMenu == INT_MAX || iMenu <= MT_NONE || iMenu > MT_BACK)
        return MT_NONE;

    return iMenu;
}

// 맵의 난이도를 출력
void OutPutBattleTag(int iMenu)
{
    switch (iMenu)
    {
    case MT_EASY:
        cout << "*************************** 쉬움 ***************************" << endl;
        break;
    case MT_NORMAL:
        cout << "*************************** 보통 ***************************" << endl;
        break;
    case MT_HARD:
        cout << "*************************** 어려움 ***************************" << endl;
        break;
    }
}

// 플레이어의 정보 출력
void OutPutPlayer(_tagPlayer *pPlayer)
{
    // 플레이어 정보를 출력한다.
    cout << "================== Player ==================" << endl;
    cout << "이름 : " << pPlayer->strName << "\t직업 : " << pPlayer->strJobName << endl;
    cout << "레벨 : " << pPlayer->iLevel << "\t경험치 : " << pPlayer->iExp << " / " << g_iLevelUpExp[pPlayer->iLevel - 1] << endl;

    // 무기를 장착하고 있을 경우 공격력에 무기공격력을 추가하여 출력한다.
    if (pPlayer->bEquip[EQ_WEAPON] == true)
    {
        cout << "공겨력 : " << pPlayer->iAttackMin << " + " << pPlayer->tEquip[EQ_WEAPON].iMin << " - " << pPlayer->iAttackMax << " + " << pPlayer->tEquip[EQ_WEAPON].iMax;
    }

    else
    {
        cout << "공격력 : " << pPlayer->iAttackMin << " - " << pPlayer->iAttackMax;
    }

    // 방어구를 장착하고 있을 경우 방어력에 방어구 방어력을 추가하여 출력한다.
    if (pPlayer->bEquip[EQ_ARMOR] == true)
    {
        cout << "\t방어력 : " << pPlayer->iArmorMin << " + " << pPlayer->tEquip[EQ_ARMOR].iMin << " - " << pPlayer->iArmorMax << " + " << pPlayer->tEquip[EQ_ARMOR].iMax << endl;
    }

    else
    {
        cout << "\t방어력 : " << pPlayer->iArmorMin << " - " << pPlayer->iArmorMax << endl;
    }

    cout << "체력 : " << pPlayer->iHP << " / " << pPlayer->iHPMax << "\t마나 : " << pPlayer->iMP << " / " << pPlayer->iMPMax << endl;

    if (pPlayer->bEquip[EQ_WEAPON])
        cout << "장착무기 : " << pPlayer->tEquip[EQ_WEAPON].strName;
    else
        cout << "장착무기 : 없음";

    if (pPlayer->bEquip[EQ_ARMOR])
        cout << "\t장착방어구 : " << pPlayer->tEquip[EQ_ARMOR].strName;
    else
        cout << "\t장착방어구 : 없음" << endl;

    cout << "보유골드 : " << pPlayer->tInventory.iGold << " Gold" << endl
         << endl;
}

// 몬스터의 정보 출력
void OutPutMonster(_tagMonster *pMonster)
{
    cout << "===================================== Monster =====================================" << endl;
    cout << "이름 : " << pMonster->strName << "\t레벨 : " << pMonster->iLevel << endl;
    cout << "공격력 : " << pMonster->iAttackMin << " - " << pMonster->iAttackMax << "\t방어력 : " << pMonster->iArmorMin << " - " << pMonster->iArmorMax << endl;
    cout << "체력 : " << pMonster->iHP << " / " << pMonster->iHPMax << "\t마나 : " << pMonster->iMP << " / " << pMonster->iMPMax << endl;
    cout << "획득경험치 : " << pMonster->iExp << "\n획득골드 : " << pMonster->iGoldMin << " - " << pMonster->iGoldMax << endl
         << endl;
}

// 맵 선택시 배틀 리스트 출력, 선택
int OutPutBattleMenu()
{
    int iMenu;
    cout << "1. 공격" << endl;
    cout << "2. 도망가기" << endl;
    cout << "메뉴를 선택하세요." << endl;
    iMenu = InputInt();

    if (iMenu == INT_MAX || iMenu < BATTLE_ATTACK || iMenu > BATTLE_BACK)
        return BATTLE_NONE;

    return iMenu;
}

// 플레이어와 몬스터 배틀
void Battle(_tagPlayer *pPlayer, _tagMonster *pMonster)
{
    int iAttackMin = pPlayer->iAttackMin;
    int iAttackMax = pPlayer->iAttackMax;

    // 무기를 장착했을 경우 무기의 Min, Max를 더해준다
    if (pPlayer->bEquip[EQ_WEAPON])
    {
        iAttackMin += pPlayer->tEquip[EQ_WEAPON].iMin;
        iAttackMax += pPlayer->tEquip[EQ_WEAPON].iMax;
    }

    // 예를 들어 Min 5 Max 15 라고 가정할 경우
    // 15 - 5 + 1 을 하면 11이 된다. 11로 나눈 나머지는 0 ~ 10이
    // 나오게 되고 여기에 Min값인 5를 더하게 되면
    // 5 ~ 15 사이로 값이 나오게 되는것이다.
    int iAttack = rand() % (iAttackMax - iAttackMin + 1) + iAttackMin;
    int iArmor = rand() % (pMonster->iArmorMax - pMonster->iArmorMin + 1) +
                 pMonster->iArmorMin;

    // 몬스터가 받은 피해량 계산
    int iDamage = iAttack - iArmor;
    iDamage = iDamage < 1 ? 1 : iDamage;

    // 몬스터의 HP를 감소시킨다.
    pMonster->iHP -= iDamage;

    cout << pPlayer->strName << " 가 " << pMonster->strName << " 에게" << iDamage << " 피해를 입혔습니다" << endl;

    // 몬스터가 죽었을 경우를 처리한다.
    if (pMonster->iHP <= 0)
    {
        cout << pMonster->strName << " 몬스터가 죽었습니다." << endl;

        pPlayer->iExp += pMonster->iExp;
        int iGold = (rand() % (pMonster->iGoldMax - pMonster->iGoldMin + 1) +
                     pMonster->iGoldMin);
        pPlayer->tInventory.iGold += iGold;

        cout << pMonster->iExp << " 경험치를 획득하였습니다." << endl;
        cout << iGold << " Gold를 획득하였습니다." << endl;

        pMonster->iHP = pMonster->iHPMax;
        pMonster->iMP = pMonster->iMPMax;

        // 플레이어의 레벨업 체크
        if (pPlayer->iExp >= g_iLevelUpExp[pPlayer->iLevel - 1])
        {
            // 경험치 초기화
            pPlayer->iExp -= g_iLevelUpExp[pPlayer->iLevel - 1];

            // 레벨을 증가시킨다.
            ++pPlayer->iLevel;

            cout << "레벨업했습니다" << endl;

            // 능력치를 상승시킨다.
            // 직업 인덱스를 구한다.
            int iJobIndex = pPlayer->eJob - 1;
            int iHPUp = rand() % (g_tLvUpTable[iJobIndex].iHPMax -
                                  g_tLvUpTable[iJobIndex].iHPMin) +
                        g_tLvUpTable[iJobIndex].iHPMin;
            int iMPUp = rand() % (g_tLvUpTable[iJobIndex].iMPMax -
                                  g_tLvUpTable[iJobIndex].iMPMin) +
                        g_tLvUpTable[iJobIndex].iMPMin;

            pPlayer->iAttackMin += g_tLvUpTable[iJobIndex].iAttackMin;
            pPlayer->iAttackMax += g_tLvUpTable[iJobIndex].iAttackMax;
            pPlayer->iArmorMin += g_tLvUpTable[iJobIndex].iArmorMin;
            pPlayer->iArmorMax += g_tLvUpTable[iJobIndex].iArmorMax;

            pPlayer->iHP += iHPUp;
            pPlayer->iMP += iMPUp;

            // 체력과 마나를 풀로 채운다.
            pPlayer->iHP = pPlayer->iHPMax;
            pPlayer->iMP = pPlayer->iMPMax;
        }

        return;
    }

    // 몬스터가 살아있다면 플레이어를 공격한다.
    iAttack = rand() % (pMonster->iAttackMax - pMonster->iAttackMin + 1) +
              pMonster->iAttackMin;

    int iArmorMin = pPlayer->iArmorMin;
    int iArmorMax = pPlayer->iArmorMax;

    if (pPlayer->bEquip[EQ_ARMOR])
    {
        iArmorMin += pPlayer->tEquip[EQ_ARMOR].iMin;
        iArmorMax += pPlayer->tEquip[EQ_ARMOR].iMax;
    }

    iArmor = rand() % (iArmorMax - iArmorMin + 1) + iArmorMin;

    iDamage = iAttack - iArmor;

    iDamage = iDamage < 1 ? 1 : iDamage;

    // 플레이어의 체력을 감소시킨다.
    pPlayer->iHP -= iDamage;

    cout << pMonster->strName << " 가 " << pPlayer->strJobName << "에게 " << iDamage << " 만큼 데미지를 입혔습니다." << endl;

    // 플레이어가 죽었을 경우
    if (pPlayer->iHP <= 0)
    {
        cout << pPlayer->strName << " 플레이어가 사망하였습니다." << endl;

        int iExp = pPlayer->iExp * 0.1f;
        int iGold = pPlayer->tInventory.iGold * 0.1f;

        pPlayer->iExp -= iExp;
        pPlayer->tInventory.iGold -= iGold;

        cout << iExp << " 경험치를 잃었습니다." << endl;
        cout << iGold << " 골드를 잃었습니다." << endl;

        pPlayer->iHP = pPlayer->iHPMax;
        pPlayer->iMP = pPlayer->iMPMax;
    }

    return;
}

void RunBattle(_tagPlayer *pPlayer, _tagMonster *pMonster, int iMenu)
{
    // 몬스터가 3마리 있으니 인덱스는 -1 해주어야 인덱스가 0, 1, 2 가 된다.
    _tagMonster tMonster = pMonster[iMenu - 1];

    while (true)
    {
        system("cls");

        // 모드 표시
        OutPutBattleTag(iMenu);

        // 플레이어 출력
        OutPutPlayer(pPlayer);

        // 맵에 따른 몬스터 출력
        OutPutMonster(&tMonster);

        switch (OutPutBattleMenu())
        {
        // 공격하기
        case BATTLE_ATTACK:
            Battle(pPlayer, &tMonster);
            system("pause");
            break;
        // 도망가기
        case BATTLE_BACK:
            return;
        }
    }
}

void RunMap(_tagPlayer *pPlayer, _tagMonster *pMonsterArr)
{
    bool bLoop = true;

    while (bLoop)
    {
        int iMenu = OutPutMapMenu();

        if (MT_BACK == iMenu)
            return;

        // 전투 시작한다
        RunBattle(pPlayer, pMonsterArr, iMenu);
    }
}

// 가게 리스트 출력, 선택
int OutPutStoreMenu()
{
    int iMenu;
    system("cls");
    cout << "******************************** 상점 **************************" << endl;
    cout << "1. 무기상점" << endl;
    cout << "2. 방어구상점" << endl;
    cout << "3. 뒤로가기" << endl;
    cout << "상점을 선택하세요. : ";
    iMenu = InputInt();

    if (iMenu == INT_MAX || iMenu <= SM_NONE || iMenu > SM_BACK)
        return SM_NONE;

    return iMenu;
}

// 판매 록록 출력, 구입할 아이템 선택 ( 현재 무기, 방어구는 3개씩 있음)
int OutPutStoreList(_tagInventory *pInventory, _tagItem *pStore, int iItemCount)
{
    // 판매 목록을 보여준다.
    for (int i = 0; i < iItemCount; i++)
    {
        cout << i + 1 << ". 이름 : " << pStore[i].strName << "\t종류 : " << pStore[i].strTypeName << endl;
        cout << "공격력 : " << pStore[i].iMin << " - " << pStore[i].iMax << endl;
        cout << "판매가격 : " << pStore[i].iPrice << "\t구매가격 : " << pStore[i].iSell << endl;
        cout << "설명 : " << pStore[i].strDesc << endl
             << endl;
    }

    cout << iItemCount + 1 << ". 뒤로가기" << endl;
    cout << "보유금액 : " << pInventory->iGold << " Gold" << endl;
    cout << "남은공간 : " << INVENTORY_MAX - pInventory->iItemCount << endl;
    cout << "구입할 아이템을 선택하세요. : ";
    int iMenu = InputInt();

    if (iMenu < 1 || iMenu > iItemCount + 1)
        return INT_MAX;

    return iMenu;
}

// 아이템 사기 함수
void BuyItem(_tagInventory *pInventory, _tagItem *pStore, int iItemCount, int iStoreType)
{
    while (true)
    {
        system("cls");
        switch (iStoreType)
        {
        case SM_WEAPON:
            cout << "*************************** 무기상점 ***************************" << endl;
            break;
        case SM_ARMOR:
            cout << "*************************** 방어구 상점 ***************************" << endl;
            break;
        }

        int iInput = OutPutStoreList(pInventory, pStore, iItemCount);

        if (iInput == INT_MAX)
        {
            cout << "잘못 입력하였습니다." << endl;
            system("pause");
            continue;
        }

        else if (iInput == iItemCount + 1)
            break;
        // 상점판매목록 배열의 인덱스를 구해준다.
        int iIndex = iInput - 1;

        // 인벤토리가 꽉 찼는지 검사한다.
        if (pInventory->iItemCount == INVENTORY_MAX)
        {
            cout << "가방이 꽉 찼습니다." << endl;
            system("pause");
            continue;
        }

        // 돈이 부족할 경우
        else if (pInventory->iGold < pStore[iIndex].iPrice)
        {
            cout << "보유금액이 부족합니다." << endl;
            system("pause");
            continue;
        }

        // 처음에 iItemCount는 하나도 추가되어있지 않기 때문에 0으로 초기화
        // 되어 있으므로 0번 인덱스에 구매한 아이템을 추가하게 된다. 그리고
        // 카운트가 1이 된다. 다음번에 추가할때는 1번 인덱스에 추가하게된다.
        pInventory->tItem[pInventory->iItemCount] =
            pStore[iIndex];
        ++pInventory->iItemCount;

        // 골드를 차감한다.
        pInventory->iGold -= pStore[iIndex].iPrice;

        cout << pStore[iIndex].strName << " 아이템을 구매하였습니다." << endl;
        system("pause");
    }
}

//상점
void RunStore(_tagInventory *pInventory, _tagItem *pWeapon, _tagItem *pArmor)
{
    while (true)
    {
        switch (OutPutStoreMenu())
        {
        case SM_WEAPON:
            BuyItem(pInventory, pWeapon, STORE_WEAPON_MAX, SM_WEAPON);
            break;
        case SM_ARMOR:
            BuyItem(pInventory, pArmor, STORE_ARMOR_MAX, SM_ARMOR);
            break;
        case SM_BACK:
            return;
        }
    }
}

//인벤토리 출력
int OutPutInventory(_tagPlayer *pPlayer)
{
    system("cls");
    cout << "*************************** 가방 ***************************" << endl;

    for (int i = 0; i < pPlayer->tInventory.iItemCount; i++)
    {
        cout << i + 1 << ". 이름 : " << pPlayer->tInventory.tItem[i].strName << "\t종류 : " << pPlayer->tInventory.tItem[i].strTypeName << endl;
        cout << "공격력 : " << pPlayer->tInventory.tItem[i].iMin << " - " << pPlayer->tInventory.tItem[i].iMax << endl;
        cout << "판매가격 : " << pPlayer->tInventory.tItem[i].iPrice << "\t구매가격 : " << pPlayer->tInventory.tItem[i].iSell << endl;
        cout << "설명 : " << pPlayer->tInventory.tItem[i].strDesc << endl
             << endl;
    }

    cout << pPlayer->tInventory.iItemCount + 1 << ". 뒤로가기" << endl;
    cout << "장착할 아이템을 선택하세요. : ";
    int iMenu = InputInt();

    if (iMenu < 1 || iMenu > pPlayer->tInventory.iItemCount + 1)
        return INT_MAX;

    return iMenu;
}

EQUIP ComputeEquipType(ITEM_TYPE eType)
{
    // 제대로 선택했을 경우 아이템의 종류에 따라 장착 부위를 결정하게 한다.
    EQUIP eq;

    switch (eType)
    {
    case IT_WEAPON:
        eq = EQ_WEAPON;
        break;
    case IT_ARMOR:
        eq = EQ_ARMOR;
        break;
    }

    return eq;
}

// 아이템 장착
void RunInventory(_tagPlayer *pPlayer)
{
    while (true)
    {
        int iInput = OutPutInventory(pPlayer);

        if (iInput == INT_MAX)
        {
            cout << "잘못 선택하였습니다." << endl;
            system("cls");
            continue;
        }

        else if (iInput == pPlayer->tInventory.iItemCount + 1)
            break;

        // 아이템 인덱스를 구해준다.
        int idx = iInput - 1;

        EQUIP eq = ComputeEquipType(pPlayer->tInventory.tItem[idx].eType);

        // 아이템이 장착되어 있을 경우 장착되어있는 아이템과 장착할 아이템을
        // 교체해주어야 한다. swap 알고리즘을 활용한다.
        if (pPlayer->bEquip[eq] == true)
        {
            _tagItem tSwap = pPlayer->tEquip[eq];
            pPlayer->tEquip[eq] = pPlayer->tInventory.tItem[idx];
            pPlayer->tInventory.tItem[idx] = tSwap;
        }

        // 장착되어 있지 않을 경우 인벤토리 아이템을 장착창으로 옮기고
        // 인벤토리는 1칸 비워지게 된다.
        else
        {
            pPlayer->tEquip[eq] = pPlayer->tInventory.tItem[idx];

            for (int i = idx; i < pPlayer->tInventory.iItemCount - 1; i++)
            {
                pPlayer->tInventory.tItem[i] = pPlayer->tInventory.tItem[i + 1];
            }

            --pPlayer->tInventory.iItemCount;

            // 장착했기 때문에 true로 만들어준다.

            pPlayer->bEquip[eq] = true;
        }

        cout << pPlayer->tEquip[eq].strName << " 아이템을 장착하였습니다." << endl;

        system("pause");
    }
}

_tagLevelUpStatus CreatLvUpStatus(int iAttackMin, int iAttackMax, int iArmorMin,
                                  int iArmorMax, int iHPMin, int iHPMax, int iMPMin, int iMPMax)
{
    _tagLevelUpStatus tStatus = {};

    tStatus.iAttackMin = iAttackMin;
    tStatus.iAttackMax = iAttackMax;
    tStatus.iArmorMin = iArmorMin;
    tStatus.iArmorMax = iArmorMax;
    tStatus.iHPMin = iHPMin;
    tStatus.iHPMax = iHPMax;
    tStatus.iMPMin = iMPMin;
    tStatus.iMPMax = iMPMax;

    return tStatus;
}

_tagItem CreateItem(const char *pName, const char *pTypeName, const char *pDesc, ITEM_TYPE eType,
                    int iMax, int iMin, int iPrice, int iSell)
{
    _tagItem tItem = {};
    strcpy_s(tItem.strName, pName);
    strcpy_s(tItem.strDesc, pDesc);

    switch (eType)
    {
    case IT_WEAPON:
        strcpy_s(tItem.strTypeName, "무기");
        break;
    case IT_ARMOR:
        strcpy_s(tItem.strTypeName, "방어구");
        break;
    }
    tItem.eType = eType;
    tItem.iMin = iMin;
    tItem.iMax = iMax;
    tItem.iPrice = iPrice;
    tItem.iSell = iSell;

    return tItem;
}

int main()
{
    srand((unsigned int)time(0));

    // 플레이어 초기화
    _tagPlayer tPlayer = {};

    // 플레이어 정보 설정
    SetPlayer(&tPlayer);

    // 몬스터 3마리를 만들기 전에 초기화 시켜준다.
    _tagMonster tMonsterArr[MT_BACK - 1] = {};

    // 몬스터 생성
    SetMonster(tMonsterArr);

    // JOB_END 는 4이다. 그런데 직업은 3개이므로 -1을 해줘서 배열을 각 직업별로
    // 생성하도록 함.
    g_tLvUpTable[JOB_KNIGHT - 1] = CreatLvUpStatus(4, 10, 8, 16, 50, 100, 10, 20);
    g_tLvUpTable[JOB_ARCHER - 1] = CreatLvUpStatus(10, 15, 5, 10, 30, 60, 30, 50);
    g_tLvUpTable[JOB_WIZARD - 1] = CreatLvUpStatus(15, 20, 3, 7, 20, 40, 50, 100);

    // 상점에서 판매할 아이템 목록을 생성한다.
    // 무기 리스트
    _tagItem tStoreWeapon[STORE_WEAPON_MAX] = {};
    // 방어구 리스트
    _tagItem tStoreArmor[STORE_ARMOR_MAX] = {};

    // 무기 생성
    tStoreWeapon[0] = CreateItem("강철검", "강철검",
                                 "용의 두꺼운 피부를 뚫고 찌를수 있다.",
                                 IT_WEAPON, 10, 5, 1000, 500);
    tStoreWeapon[1] = CreateItem("불화살", "화살 종류",
                                 "불화살이 날라가서 지속 데미지를 입힌다.",
                                 IT_WEAPON, 40, 20, 7000, 3500);
    tStoreWeapon[2] = CreateItem("나무지팡이", "지팡이 종류",
                                 "마법을 사용할때 공격력이 더 쌔진다.",
                                 IT_WEAPON, 150, 90, 30000, 15000);

    // 방어구 생성
    tStoreArmor[0] = CreateItem("강철 갑옷", "몸통 방어구 종류",
                                "방어력이 높아진다.",
                                IT_ARMOR, 2, 5, 1000, 500);
    tStoreArmor[1] = CreateItem("불 갑옷", "몸통 방어구 종류",
                                "데미지를 적게 받는 효과가 있다",
                                IT_ARMOR, 20, 10, 7000, 3500);
    tStoreArmor[2] = CreateItem("수은 갑옷", "몸통 방어구 종류",
                                "쉽게 뚫지 못한다.",
                                IT_ARMOR, 90, 70, 30000, 15000);

    // 로비 보여주기
    bool bLoop = true;

    while (bLoop)
    {
        switch (OutPutMenu())
        {
        // 맵을 선택했을 경우
        case MM_MAP:
            RunMap(&tPlayer, tMonsterArr);
            break;
        // 상점을 선택했을 경우
        case MM_STORE:
            RunStore(&tPlayer.tInventory, tStoreWeapon, tStoreArmor);
            break;
        // 인벤토리(가방)을 선택했을 경우
        case MM_INVENTORY:
            RunInventory(&tPlayer);
            break;
        // 종료 선택시 while문 탈출
        case MM_EXIT:
            bLoop = false;
            break;
        }
    }

    return 0;
}