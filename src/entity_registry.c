#include "Nori.h"

void er_init(entity_registry *pEr, u32 cvecCapacity)
{
    size_t cvecsize = cvecCapacity * sizeof(cvec);
    pEr->cVectors = (cvec*)MALLOC(cvecsize);
    pEr->maxID = 0;
    pEr->cvecCount = 0;
    pEr->cvecCapacity = cvecCapacity;
    idq_init(&pEr->idQueue);
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
    cvec *pCv = &pEr->cVectors[componentID];

    return cv_find(pCv, entityID);
}

void write_to_buffer(void** restrict buffer, const void* restrict src, size_t size)
{
    memcpy(*buffer, src, size);
    *buffer += size;
}

void er_serialize(entity_registry *pEr, const char *filePath)
{
    //TODO: serialize the id queue
    FILE *pFile = fopen(filePath, "wb");
    if (pFile)
    {
        size_t size = sizeof(entity_registry);
        for(int i = 0; i < pEr->cvecCount; i++)
            size += cv_sizeof(&pEr->cVectors[i]);

        id_node* ptr = pEr->idQueue.front;
        while(ptr)
        {
            size += sizeof(u32);
            ptr = ptr->next;
        }

        printf("%lu\n",size);
        char* buffer = MALLOC(size);
        const char* const end = buffer + size; 
        char* writer = buffer;

        write_to_buffer(&writer, &pEr->cvecCount, sizeof(u32)); // component count
        write_to_buffer(&writer, &pEr->maxID, sizeof(u32)); // maxID


        for(u32 i = 0; i < pEr->cvecCount; i++)
        {
            cvec* pCv = er_get_cvec(pEr,i);
            const size_t cvecSize = (size_t)pCv->componentCount * (size_t)pCv->componentSize;
            const size_t denseSize = (size_t)pCv->entitySet.denseCount * sizeof(u32);
            if(pCv->componentCount != pCv->entitySet.denseCount)
                fprintf(stderr,"Serialization Error, componentID %u! Component count != Sparse set count\n", i);
            write_to_buffer(&writer, &pCv->componentSize, sizeof(pCv->componentSize)); // component size
            write_to_buffer(&writer, &pCv->componentCount, sizeof(u32)); // component count
            write_to_buffer(&writer,pCv->components, cvecSize); // Components
            write_to_buffer(&writer, pCv->entitySet.dense, denseSize); // Entity IDs
        }
        fwrite(&writer, 1,size,pFile);
        fclose(pFile);
        FREE(buffer);
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

        byte* buffer = MALLOC(size);
        byte* reader = buffer;

        fread(buffer, 1, size,pFile);
        sscanf(reader,"%u%u", &pEr->cvecCount, &pEr->maxID);
        reader += 2 * sizeof(u32);

        pEr->cVectors = MALLOC(pEr->cvecCount * sizeof(cvec));

        for(u32 i = 0; i < pEr->cvecCount; i++)
        {
            cvec* pCv = &pEr->cVectors[i];
            u32 eSize, cCount;

            sscanf(reader, "")

        }        

        FREE(buffer);
        fclose(pFile);
    }
    else
    {
        fprintf(stderr, "File %s not found!", filePath);
    }
}