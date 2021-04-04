#include "Nori.h"

void er_init(entity_registry *pEr, u32 cvecCapacity)
{
    size_t cvecsize = cvecCapacity * sizeof(cvec);
    pEr->cVectors = (cvec*)MALLOC(cvecsize);
    pEr->maxID = 0;
    pEr->cvecCount = 0;
    pEr->cvecCapacity = cvecCapacity;
    idq_init(&pEr->idQueue, 128);
}



u32 er_create_entity(entity_registry *pEr)
{
    u32 retval;
    if (!idq_is_empty(&pEr->idQueue))
    {
        idq_pop(&pEr->idQueue, &retval);
    }
    else
    {
        retval = pEr->maxID;
        pEr->maxID++;
    }
    return retval;
}

cvec* er_get_cvec(entity_registry* pEr, componentID_t componentID)
{
#ifdef DEBUG
    if (pEr->cvecCount <= componentID)
    {
        fprintf(stderr, "ComponentID %u does not exist\n", componentID);
        return NULL;
    }
#endif
    return &pEr->cVectors[componentID];
}

void er_free(entity_registry *pEr)
{
    for (int i = 0; i < pEr->cvecCount; i++)
    {
        cv_free(&pEr->cVectors[i]);
    }
    FREE(pEr->cVectors);

    idq_free(&pEr->idQueue);
}

void er_push_component(entity_registry *pEr, entity_t entityID, componentID_t componentID, const void *pComponent)
{

    cvec *pCv = &pEr->cVectors[componentID];
    cv_push(pCv, pComponent, entityID);
}

void *er_emplace_component(entity_registry *pEr, entity_t entityID, componentID_t componentID)
{
    cvec *pCv = &pEr->cVectors[componentID];

    return cv_emplace(pCv, entityID);
}

void er_remove_entity(entity_registry *pEr, entity_t entityID)
{
    cvec *pVecs = pEr->cVectors;
    const cvec *const pEnd = pEr->cVectors + pEr->cvecCount;

    for (; pVecs != pEnd; pVecs++)
    {
        if (cv_find(pVecs, entityID) != NULL)
            cv_remove(pVecs, entityID);
    }
    idq_push(&pEr->idQueue, entityID);
}

entity_t er_add_cvec(entity_registry *pEr, u32 componentSize, u32 InitialCount)
{
#ifdef DEBUG
    if (pEr->cvecCount == pEr->cvecCapacity)
    {
        fprintf(stderr, "ERROR IN FILE %s, LINE %d:\nNot enough reserved space for another cvector!\n", __FILE__, __LINE__);
        return -1;
    }
#endif
    printf("pEr->cvecCount: %u\n", pEr->cvecCount);
    cv_init(&pEr->cVectors[pEr->cvecCount], componentSize, InitialCount);
    return pEr->cvecCount++;
}

void *er_get_component(entity_registry *pEr, entity_t entityID, componentID_t componentID)
{
    cvec *pCv = er_get_cvec(pEr,componentID);

    return cv_find(pCv, entityID);
}

void er_serialize(entity_registry *pEr, const char *filePath)
{
    //TODO: serialize the id queue
    FILE *pFile = fopen(filePath, "wb");
    if (pFile)
    {

        fwrite(&pEr->cvecCount, sizeof(u32),1, pFile);
        fwrite(&pEr->maxID,sizeof(u32), 1, pFile);
        
        for(u32 i = 0; i < pEr->cvecCount; i++)
        {
            cvec* pCv = er_get_cvec(pEr,i);
            if(pCv->componentCount != pCv->entitySet.denseCount)
                fprintf(stderr,"Serialization Error, componentID %u! Component count != Sparse set count\n", i);

            fwrite(&pCv->componentSize, sizeof(u32) , 1, pFile); // Component Size
            fwrite(&pCv->componentCount, sizeof(u32), 1, pFile); // Component Count
            fwrite(pCv->components, pCv->componentSize, pCv->componentCount, pFile); // Components  
            fwrite(pCv->entitySet.dense, sizeof(u32), pCv->componentCount, pFile);    // Entity IDs
        }

        fwrite(&v_size(pEr->idQueue.queue), sizeof(u32), 1, pFile);
        fwrite(pEr->idQueue.queue, sizeof(u32), v_size(pEr->idQueue.queue),pFile);

        fclose(pFile);
    } else
    {
        fprintf(stderr, "File %s not found!", filePath);
    }    
}


void er_deserialize(entity_registry* pEr, const char* filePath)
{
    FILE* pFile = fopen(filePath, "rb");
    if(pFile)
    {
        fseek(pFile,0, SEEK_END);
        size_t size = ftell(pFile);
        fseek(pFile,0, SEEK_SET);

        fread(&pEr->cvecCount, sizeof(u32), 1, pFile);
        fread(&pEr->maxID, sizeof(u32), 1, pFile);

        pEr->cVectors = MALLOC(pEr->cvecCount * sizeof(cvec));

        for(u32 i = 0; i < pEr->cvecCount; i++)
        {
            cvec* pCv = &pEr->cVectors[i];
            u32 eSize, cCount;

            fread(&eSize,sizeof(u32), 1, pFile);
            fread(&cCount, sizeof(u32), 1, pFile);

            cv_init(pCv,eSize,cCount);

            pCv->componentCount = cCount;
            pCv->entitySet.denseCount = cCount;
            fread(pCv->components, eSize, cCount, pFile);
            fread(pCv->entitySet.dense, sizeof(u32), cCount, pFile);
            recalculate_sparse(&pCv->entitySet);
        }        
        u32 qSize;
        fread(&qSize, sizeof(u32), 1, pFile);
        idq_init(&pEr->idQueue, qSize);
        fread(pEr->idQueue.queue, sizeof(u32), qSize,pFile);
        v_size(pEr->idQueue.queue) = qSize;
        fclose(pFile);
    }
    else
    {
        fprintf(stderr, "File %s not found!", filePath);
    }
}
