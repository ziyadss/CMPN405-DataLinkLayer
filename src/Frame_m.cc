//
// Generated file, do not edit! Created by nedtool 5.7 from Frame.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#pragma warning(disable : 4101)
#pragma warning(disable : 4065)
#endif

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wshadow"
#pragma clang diagnostic ignored "-Wconversion"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wc++98-compat"
#pragma clang diagnostic ignored "-Wunreachable-code-break"
#pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include "Frame_m.h"

namespace omnetpp
{

    // Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
    // They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

    // Packing/unpacking an std::vector
    template <typename T, typename A>
    void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T, A> &v)
    {
        int n = v.size();
        doParsimPacking(buffer, n);
        for (int i = 0; i < n; i++)
            doParsimPacking(buffer, v[i]);
    }

    template <typename T, typename A>
    void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T, A> &v)
    {
        int n;
        doParsimUnpacking(buffer, n);
        v.resize(n);
        for (int i = 0; i < n; i++)
            doParsimUnpacking(buffer, v[i]);
    }

    // Packing/unpacking an std::list
    template <typename T, typename A>
    void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T, A> &l)
    {
        doParsimPacking(buffer, (int)l.size());
        for (typename std::list<T, A>::const_iterator it = l.begin(); it != l.end(); ++it)
            doParsimPacking(buffer, (T &)*it);
    }

    template <typename T, typename A>
    void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T, A> &l)
    {
        int n;
        doParsimUnpacking(buffer, n);
        for (int i = 0; i < n; i++)
        {
            l.push_back(T());
            doParsimUnpacking(buffer, l.back());
        }
    }

    // Packing/unpacking an std::set
    template <typename T, typename Tr, typename A>
    void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T, Tr, A> &s)
    {
        doParsimPacking(buffer, (int)s.size());
        for (typename std::set<T, Tr, A>::const_iterator it = s.begin(); it != s.end(); ++it)
            doParsimPacking(buffer, *it);
    }

    template <typename T, typename Tr, typename A>
    void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T, Tr, A> &s)
    {
        int n;
        doParsimUnpacking(buffer, n);
        for (int i = 0; i < n; i++)
        {
            T x;
            doParsimUnpacking(buffer, x);
            s.insert(x);
        }
    }

    // Packing/unpacking an std::map
    template <typename K, typename V, typename Tr, typename A>
    void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K, V, Tr, A> &m)
    {
        doParsimPacking(buffer, (int)m.size());
        for (typename std::map<K, V, Tr, A>::const_iterator it = m.begin(); it != m.end(); ++it)
        {
            doParsimPacking(buffer, it->first);
            doParsimPacking(buffer, it->second);
        }
    }

    template <typename K, typename V, typename Tr, typename A>
    void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K, V, Tr, A> &m)
    {
        int n;
        doParsimUnpacking(buffer, n);
        for (int i = 0; i < n; i++)
        {
            K k;
            V v;
            doParsimUnpacking(buffer, k);
            doParsimUnpacking(buffer, v);
            m[k] = v;
        }
    }

    // Default pack/unpack function for arrays
    template <typename T>
    void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
    {
        for (int i = 0; i < n; i++)
            doParsimPacking(b, t[i]);
    }

    template <typename T>
    void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
    {
        for (int i = 0; i < n; i++)
            doParsimUnpacking(b, t[i]);
    }

    // Default rule to prevent compiler from choosing base class' doParsimPacking() function
    template <typename T>
    void doParsimPacking(omnetpp::cCommBuffer *, const T &t)
    {
        throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
    }

    template <typename T>
    void doParsimUnpacking(omnetpp::cCommBuffer *, T &t)
    {
        throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
    }

} // namespace omnetpp

// forward
template <typename T, typename A>
std::ostream &operator<<(std::ostream &out, const std::vector<T, A> &vec);

// Template rule which fires if a struct or class doesn't have operator<<
template <typename T>
inline std::ostream &operator<<(std::ostream &out, const T &) { return out; }

// operator<< for std::vector<T>
template <typename T, typename A>
inline std::ostream &operator<<(std::ostream &out, const std::vector<T, A> &vec)
{
    out.put('{');
    for (typename std::vector<T, A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin())
        {
            out.put(',');
            out.put(' ');
        }
        out << *it;
    }
    out.put('}');

    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

Frame_Base::Frame_Base(const char *name, short kind) : ::omnetpp::cPacket(name, kind)
{
    this->trailer = 0;
    this->ack = false;
    this->piggyback_id = 0;
}

Frame_Base::Frame_Base(const Frame_Base &other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

Frame_Base::~Frame_Base()
{
}

Frame_Base::Frame_Base(const Header &header, const char *payload, char trailer, bool ack, unsigned int piggyback_id) : ::omnetpp::cPacket(nullptr, 0)
{
    this->header = header;
    this->payload = payload;
    this->trailer = trailer;
    this->ack = ack;
    this->piggyback_id = piggyback_id;
}

Frame_Base &Frame_Base::operator=(const Frame_Base &other)
{
    if (this == &other)
        return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void Frame_Base::copy(const Frame_Base &other)
{
    this->header = other.header;
    this->payload = other.payload;
    this->trailer = other.trailer;
    this->ack = other.ack;
    this->piggyback_id = other.piggyback_id;
}

void Frame_Base::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b, this->header);
    doParsimPacking(b, this->payload);
    doParsimPacking(b, this->trailer);
    doParsimPacking(b, this->ack);
    doParsimPacking(b, this->piggyback_id);
}

void Frame_Base::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b, this->header);
    doParsimUnpacking(b, this->payload);
    doParsimUnpacking(b, this->trailer);
    doParsimUnpacking(b, this->ack);
    doParsimUnpacking(b, this->piggyback_id);
}

Header &Frame_Base::getHeader()
{
    return this->header;
}

void Frame_Base::setHeader(const Header &header)
{
    this->header = header;
}

const char *Frame_Base::getPayload() const
{
    return this->payload.c_str();
}

void Frame_Base::setPayload(const char *payload)
{
    this->payload = payload;
}

char Frame_Base::getTrailer() const
{
    return this->trailer;
}

void Frame_Base::setTrailer(char trailer)
{
    this->trailer = trailer;
}

bool Frame_Base::getAck() const
{
    return this->ack;
}

void Frame_Base::setAck(bool ack)
{
    this->ack = ack;
}

unsigned int Frame_Base::getPiggyback_id() const
{
    return this->piggyback_id;
}

void Frame_Base::setPiggyback_id(unsigned int piggyback_id)
{
    this->piggyback_id = piggyback_id;
}

class FrameDescriptor : public omnetpp::cClassDescriptor
{
private:
    mutable const char **propertynames;

public:
    FrameDescriptor();
    virtual ~FrameDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(FrameDescriptor)

    FrameDescriptor::FrameDescriptor() : omnetpp::cClassDescriptor("Frame", "omnetpp::cPacket")
{
    propertynames = nullptr;
}

FrameDescriptor::~FrameDescriptor()
{
    delete[] propertynames;
}

bool FrameDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<Frame_Base *>(obj) != nullptr;
}

const char **FrameDescriptor::getPropertyNames() const
{
    if (!propertynames)
    {
        static const char *names[] = {"customize", nullptr};
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *FrameDescriptor::getProperty(const char *propertyname) const
{
    if (!strcmp(propertyname, "customize"))
        return "true";
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int FrameDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 5 + basedesc->getFieldCount() : 5;
}

unsigned int FrameDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc)
    {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISCOMPOUND,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field >= 0 && field < 5) ? fieldTypeFlags[field] : 0;
}

const char *FrameDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc)
    {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "header",
        "payload",
        "trailer",
        "ack",
        "piggyback_id",
    };
    return (field >= 0 && field < 5) ? fieldNames[field] : nullptr;
}

int FrameDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0] == 'h' && strcmp(fieldName, "header") == 0)
        return base + 0;
    if (fieldName[0] == 'p' && strcmp(fieldName, "payload") == 0)
        return base + 1;
    if (fieldName[0] == 't' && strcmp(fieldName, "trailer") == 0)
        return base + 2;
    if (fieldName[0] == 'a' && strcmp(fieldName, "ack") == 0)
        return base + 3;
    if (fieldName[0] == 'p' && strcmp(fieldName, "piggyback_id") == 0)
        return base + 4;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *FrameDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc)
    {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "Header",
        "string",
        "char",
        "bool",
        "unsigned int",
    };
    return (field >= 0 && field < 5) ? fieldTypeStrings[field] : nullptr;
}

const char **FrameDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc)
    {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field)
    {
    default:
        return nullptr;
    }
}

const char *FrameDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc)
    {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field)
    {
    default:
        return nullptr;
    }
}

int FrameDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc)
    {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    Frame_Base *pp = (Frame_Base *)object;
    (void)pp;
    switch (field)
    {
    default:
        return 0;
    }
}

const char *FrameDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc)
    {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object, field, i);
        field -= basedesc->getFieldCount();
    }
    Frame_Base *pp = (Frame_Base *)object;
    (void)pp;
    switch (field)
    {
    default:
        return nullptr;
    }
}

std::string FrameDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc)
    {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object, field, i);
        field -= basedesc->getFieldCount();
    }
    Frame_Base *pp = (Frame_Base *)object;
    (void)pp;
    switch (field)
    {
    case 0:
    {
        std::stringstream out;
        out << pp->getHeader();
        return out.str();
    }
    case 1:
        return oppstring2string(pp->getPayload());
    case 2:
        return long2string(pp->getTrailer());
    case 3:
        return bool2string(pp->getAck());
    case 4:
        return ulong2string(pp->getPiggyback_id());
    default:
        return "";
    }
}

bool FrameDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc)
    {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object, field, i, value);
        field -= basedesc->getFieldCount();
    }
    Frame_Base *pp = (Frame_Base *)object;
    (void)pp;
    switch (field)
    {
    case 1:
        pp->setPayload((value));
        return true;
    case 2:
        pp->setTrailer(string2long(value));
        return true;
    case 3:
        pp->setAck(string2bool(value));
        return true;
    case 4:
        pp->setPiggyback_id(string2ulong(value));
        return true;
    default:
        return false;
    }
}

const char *FrameDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc)
    {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field)
    {
    case 0:
        return omnetpp::opp_typename(typeid(Header));
    default:
        return nullptr;
    };
}

void *FrameDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc)
    {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    Frame_Base *pp = (Frame_Base *)object;
    (void)pp;
    switch (field)
    {
    case 0:
        return (void *)(&pp->getHeader());
        break;
    default:
        return nullptr;
    }
}
